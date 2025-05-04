#include "http_request_builder.h"
#include "http/http_body_stream_reader.h"
#include "http/url_dispatcher.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace empty_d::http::request {

HttpRequestBuilder::HttpRequestBuilder(
    std::shared_ptr<UrlDispatcher> urlDispatcher,
    const boost::asio::any_io_executor &executor)
    : mUrdispatcher(std::move(urlDispatcher)), mExecutor(executor) {}

void HttpRequestBuilder::AppendHeaderField(const std::string &header_field) {
  current_header_field_ = header_field;
}

void HttpRequestBuilder::AppendHeaderValue(const std::string &header_value) {
  if (current_header_field_) {
    headers_.add(current_header_field_.value(), header_value);
  } else {
    throw std::runtime_error("header require");
  }
}

void HttpRequestBuilder::AppendHttpVersion(const std::string &http_version) {
  http_version_ = http_version;
}

void HttpRequestBuilder::AppendMethod(http::HttpMethods method) {
  method_ = method;
}

void HttpRequestBuilder::AppendPath(const std::string &path) { path_ = path; }

void HttpRequestBuilder::AppendQuery(
    const std::unordered_map<std::string, std::string> &query) {
  query_ = query;
}

void HttpRequestBuilder::AppendHeader(const std::string &header_field,
                                      const std::string &header_value) {
  headers_.add(header_field, header_value);
}

void HttpRequestBuilder::AppendBody(std::string body) {
  if (!body_reader_) {
    throw std::runtime_error("need body reader");
  }
  body_reader_->write(body.begin(), body.end());
}

void HttpRequestBuilder::onHeadersComplete() {
  if (headers_.hasContentLength()) {
    body_reader_ = std::make_shared<request::HttpBodyStreamReader>(
        mExecutor, headers_.getContentLength());
  }
  
}

std::pair<HttpRequest, HttpHandler> HttpRequestBuilder::BuildRequest() {
  std::string path = path_.value();
  resource_ = mUrdispatcher->getResource(path);
  if (resource_) {
    const auto &path_args = resource_->getPathArgs();
    for (auto &path_arg : path_args) {
      std::string::size_type arg_last_pos =
          path.find('/', path_arg.segment_pos);
      if (arg_last_pos == std::string::npos) {
        arg_last_pos = path.size();
      }
      path_args_[path_arg.arg_name] =
          path.substr(path_arg.segment_pos, arg_last_pos);
    }
  } else {
    throw std::runtime_error("no match path");
  }

  HttpHandler handler = nullptr;
  if (resource_ && method_) {
    handler = resource_->getHandler(method_.value());
  } else {
    throw std::runtime_error("need resource or method");
  }

  if (!(path_ && http_version_)) {
    throw std::runtime_error("need path or version");
  }

  size_t content_length = headers_.getContentLength();
  const std::string &host = headers_.getHost();
  return {HttpRequest{content_length, method_.value(), std::move(headers_),
                      host, std::move(http_version_.value()),
                      std::move(body_reader_), std::move(path_.value()),
                      std::move(query_), std::move(path_args_)},
          handler};
}

boost::optional<Resource> HttpRequestBuilder::GetResource() const {
  return resource_;
}

} // namespace empty_d::http::request
