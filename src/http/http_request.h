#pragma once

#include "definitions/strings/string_traits.h"
#include "http/http_body_stream_reader.h"
#include "http/http_headers.h"
#include "http/http_methods.h"
#include <boost/asio/spawn.hpp>
#include <boost/json.hpp>
#include <boost/json/value_to.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace empty_d::http::request {

using namespace boost;

class HttpRequest {
public:
  using QueryArgs = std::unordered_map<std::string, std::string>;
  using PathArgs = std::unordered_map<std::string, std::string>;

  HttpRequest(size_t content_length, HttpMethods method_, HttpHeaders headers,
              std::string host, std::string http_version,
              std::shared_ptr<HttpBodyStreamReader> body, std::string path,
              QueryArgs query_args, PathArgs path_args);

  template <typename T> T ReadBody(boost::asio::yield_context yield);

  json::value readJson(boost::asio::yield_context yield);

  std::string readAny(boost::asio::yield_context yield);

  std::string readAll(boost::asio::yield_context yield);

  std::size_t getContentLength() const;
  const std::string &getPath() const;
  http::HttpMethods getMethod() const;
  std::shared_ptr<HttpBodyStreamReader> getStreamReader() const;
  const std::vector<std::string> &getHeaders(const std::string &key) const;
  const std::string &getUrl() const;
  const std::string &getHeader(const std::string &headerName) const;
  const std::string &getHttpVersion() const;
  const std::string &getHost() const;
  const std::string &getRemoteIp() const;

  template <typename T> T getQueryArg(const std::string &argName) const;
  template <typename T>
  T getQueryArg(const std::string &argName, const T &defaultParam) const;

  template <typename T> T getPathArg(const std::string &argName) const;
  template <typename T>
  T getPathArg(const std::string &argName, const T &defaultParam) const;

private:
  QueryArgs mQueryArgs;
  PathArgs mPathArgs;
  std::size_t mcontentLength;
  HttpMethods mMethod;
  HttpHeaders mHeaders;
  std::string mHost;
  std::string mHttpVersion;
  std::shared_ptr<HttpBodyStreamReader> mStreamReader;
  std::string mPath;
};

template <typename T>
T HttpRequest::ReadBody(boost::asio::yield_context yield) {
  return boost::json::value_to<T>(readJson(yield));
}

template <typename T>
T HttpRequest::getQueryArg(const std::string &argName) const {
  auto it = mQueryArgs.find(argName);
  if (it == mQueryArgs.end()) {
    throw std::runtime_error("Arg not found: " + argName);
  }

  try {
    return helpers::StringTraits<T>::convert(it->second);
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string(e.what()) + " (arg: " + argName + ")");
  }
}

template <typename T>
T HttpRequest::getQueryArg(const std::string &argName,
                           const T &defaultParam) const {
  auto it = mQueryArgs.find(argName);
  if (it == mQueryArgs.end()) {
    return defaultParam;
  }

  try {
    return helpers::StringTraits<T>::convert(it->second);
  } catch (const std::exception &) {
    return defaultParam;
  }
}

} // namespace empty_d::http::request