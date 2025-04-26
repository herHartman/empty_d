#pragma once

#include "http/http_headers.h"
#include "http/protocol/http_body_stream_reader.h"
#include "http/protocol/http_methods.h"
#include <boost/asio/spawn.hpp>
#include <boost/json.hpp>
#include <boost/json/value_to.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


namespace empty_d::http::request {

using namespace boost;

class HttpRequest {
public:
  using QueryArgs = std::unordered_map<std::string, std::string>;
  using PathArgs = std::unordered_map<std::string, std::string>;

  explicit HttpRequest(size_t content_length, HttpMethods method_,
                       HttpHeaders headers, std::string host,
                       std::string http_version,
                       std::shared_ptr<HttpBodyStreamReader> body,
                       std::string path, QueryArgs query_args,
                       PathArgs path_args)
      : mcontentLength(content_length), mMethod(method_),
        mHeaders(std::move(headers)), mHost(std::move(host)),
        mHttpVersion(std::move(http_version)), mStreamReader(std::move(body)),
        mPath(std::move(path)), mQueryArgs(std::move(query_args)),
        mPathArgs(std::move(path_args)), mBuffer{} {}

  template <typename T> T ReadBody(boost::asio::yield_context yield) {
    return boost::json::value_to<T>(readJson(yield));
  }

  json::value readJson(boost::asio::yield_context yield) {
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

  std::string readAny(boost::asio::yield_context yield) { return mStreamReader->readAny(yield); }

  std::string readAll(boost::asio::yield_context yield) {
    std::string buffer;
    while (!mStreamReader->isEof()) {
      buffer.append(mStreamReader->readAny(yield));
    }
    return buffer;
  }
  
  [[nodiscard]] std::size_t GetContentLength() const { return mcontentLength; }

  [[nodiscard]] const std::string &GetPath() const { return mPath; }

  [[nodiscard]] http::HttpMethods GetMethod() const { return mMethod; }

  [[nodiscard]] std::shared_ptr<HttpBodyStreamReader> GetStreamReader() const {
    return mStreamReader;
  }

  const std::vector<std::string> &GetHeaders(const std::string &key);

private:
  std::size_t mcontentLength;
  HttpMethods mMethod;
  HttpHeaders mHeaders;
  std::string mHost;
  std::string mHttpVersion;
  std::shared_ptr<HttpBodyStreamReader> mStreamReader;
  std::vector<char> mBuffer;
  std::string mPath;
  std::unordered_map<std::string, std::string> mQueryArgs;
  std::unordered_map<std::string, std::string> mPathArgs;
};
} // namespace empty_d::http::request