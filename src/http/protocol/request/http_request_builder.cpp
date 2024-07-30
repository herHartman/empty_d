#include "http_request_builder.h"
#include "http/url_dispatcher.hpp"
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace empty_d::http::request {

void HttpRequestBuilder::AppendHeaderField(const std::string &header_field) {
  current_header_field_ = header_field;
}

void HttpRequestBuilder::AppendHeaderValue(const std::string &header_value) {
  if (current_header_field_) {
    headers_.Add(current_header_field_.value(), header_value);
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

void HttpRequestBuilder::AppendPath(const std::string &path) {
  path_ = path;
  resource_ = url_dispatcher_->GetResource(path);
  if (resource_) {
    const auto &path_args = resource_->GetPathArgs();
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
}

void HttpRequestBuilder::AppendQuery(
    const std::unordered_map<std::string, std::string> &query) {
  query_ = query;
}

void HttpRequestBuilder::AppendHeader(const std::string &header_field,
                                      const std::string &header_value) {
  headers_.Add(header_field, header_value);
}

HttpRequest HttpRequestBuilder::BuildRequest() {
  HttpHandler handler = nullptr;
  if (resource_ && method_) {
    handler = resource_->GetHandler(method_.value());
  } else {
    throw std::runtime_error("need resource or method");
  }

  if (!(path_ && http_version_)) {
    throw std::runtime_error("need path or version");
  }

  size_t content_length = headers_.GetContentLength();
  std::string &host = headers_.GetHost();
  return HttpRequest{content_length,
                     method_.value(),
                     std::move(headers_),
                     host,
                     std::move(http_version_.value()),
                     std::shared_ptr<HttpBodyStreamReader>(nullptr),
                     std::move(path_.value()),
                     std::move(query_),
                     std::move(path_args_)};
}

} // namespace empty_d::http::request
