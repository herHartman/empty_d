#pragma once

#include "definitions/boost_asio_types.h"
#include "http/protocol/http_body_stream_reader.h"
#include "http/protocol/http_methods.h"
#include "http/http_headers.h"
#include "io/serializable/json_serializable.h"
#include <boost/json.hpp>
#include <string>

using namespace boost;

namespace empty_d::http::request {
class HttpRequest {
public:
  // explicit HttpRequest(
  //   size_t content_length, const std::string& content_type,
  //   HttpMethods http_method, 
  //   std::shared_ptr<HttpBodyStreamReader> stream_reader
  // )
  //     : stream_reader_(std::move(stream_reader)) {}

  template <is_serializable T> awaitable<T> ReadBody() {
    co_return typename T::serializer{}.deserialize(co_await ReadJson());
  }

  awaitable<json::value> ReadJson() {
    const char *data = co_await Read();
    boost::system::error_code ec;
    json::value json_value = json::parse(data, ec);
    co_return json_value;
  }

  awaitable<const char *> Read() {
    if (buffer_.empty()) {
      while (true) {
        buffer_.reserve(GetContentLength());
        std::vector<char> chunk = co_await stream_reader_->ReadAny();
        if (chunk.empty())
          break;
        buffer_.assign(chunk.begin(), chunk.end());
      }
    }
    co_return buffer_.data();
  }

  [[nodiscard]] std::size_t GetContentLength() const { return content_length_; }

  [[nodiscard]] std::string GetPath() const { return path_; }

  [[nodiscard]] http::HttpMethods GetMethod() const { return method_; }

  [[nodiscard]] std::shared_ptr<HttpBodyStreamReader> GetStreamReader() const {
    return stream_reader_;
  }

  const std::vector<std::string> &GetHeaders(std::string_view key);

private:
  std::size_t content_length_;
  std::string content_type_;

  HttpMethods method_;
  HttpHeaders headers_;
  std::string host_;
  std::string url_;
  std::string version_;
  std::shared_ptr<HttpBodyStreamReader> stream_reader_;
  std::vector<char> buffer_;
  std::string path_;
};
} // namespace empty_d::http::request
