#pragma once

#include "http/http_request.h"

namespace empty_d::http::request {

HttpRequest::HttpRequest(size_t content_length, HttpMethods method_,
                         HttpHeaders headers, std::string host,
                         std::string http_version,
                         std::shared_ptr<HttpBodyStreamReader> body,
                         std::string path, QueryArgs query_args,
                         PathArgs path_args)
    : mcontentLength(content_length), mMethod(method_),
      mHeaders(std::move(headers)), mHost(std::move(host)),
      mHttpVersion(std::move(http_version)), mStreamReader(std::move(body)),
      mPath(std::move(path)), mQueryArgs(std::move(query_args)),
      mPathArgs(std::move(path_args)) {}

json::value HttpRequest::readJson(boost::asio::yield_context yield) {
  boost::json::stream_parser jsonParser;
  while (!mStreamReader->isEof()) {
    jsonParser.write(mStreamReader->readAny(yield));
  }
  if (jsonParser.done()) {
    return jsonParser.release();
  } else {
    throw std::runtime_error("error parse json");
  }
}

std::string HttpRequest::readAny(boost::asio::yield_context yield) {
  return mStreamReader->readAny(yield);
}

std::string HttpRequest::readAll(boost::asio::yield_context yield) {
  std::string buffer;
  while (!mStreamReader->isEof()) {
    buffer.append(mStreamReader->readAny(yield));
  }
  return buffer;
}

std::size_t HttpRequest::getContentLength() const { return mcontentLength; }

const std::string &HttpRequest::getPath() const { return mPath; }

HttpMethods HttpRequest::getMethod() const { return mMethod; }

std::shared_ptr<HttpBodyStreamReader> HttpRequest::getStreamReader() const {
  return mStreamReader;
}

const std::vector<std::string> &
HttpRequest::getHeaders(const std::string &key) const {}

const std::string &HttpRequest::getUrl() const {}

const std::string &HttpRequest::getHeader(const std::string &headerName) const {

}

const std::string &HttpRequest::getHttpVersion() const {}

const std::string &HttpRequest::getHost() const {}

const std::string &HttpRequest::getRemoteIp() const {}

} // namespace empty_d::http::request