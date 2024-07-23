#pragma once

#include "definitions/boost_asio_types.h"
#include "http/protocol/http_body_stream_reader.h"
#include "http/protocol/http_methods.h"
#include "io/serializable/json_serializable.h"
#include <boost/json.hpp>
#include <string>

using namespace boost;

namespace empty_d::http::request {
class HttpRequest {
public:
  explicit HttpRequest(std::shared_ptr<HttpBodyStreamReader> stream_reader)
      : stream_reader_(std::move(stream_reader)) {}

  template <is_serializable T> awaitable<T> read_body() {
    co_return typename T::serializer{}.deserialize(co_await read_json());
  }

  awaitable<json::value> read_json() {
    const char *data = co_await read();
    boost::system::error_code ec;
    json::value json_value = json::parse(data, ec);
    co_return json_value;
  }

  awaitable<const char *> read() {
    if (buffer_.empty()) {
      while (true) {
        buffer_.reserve(get_content_length());
        std::vector<char> chunk = co_await stream_reader_->ReadAny();
        if (chunk.empty())
          break;
        buffer_.assign(chunk.begin(), chunk.end());
      }
    }
    co_return buffer_.data();
  }

  [[nodiscard]] std::size_t get_content_length() {
    // if (!content_length_) {
    //     content_length_ = request_message_.get_content_length();
    // }
    return content_length_.value();
  }

  [[nodiscard]] std::string get_path() {
    // if (!path_) {
    //     path_ = request_message_.path;
    // }
    return path_.value();
  }

  [[nodiscard]] http::HttpMethods get_method() const { return method_; }

  [[nodiscard]] std::shared_ptr<HttpBodyStreamReader>
  get_stream_reader() const {
    return stream_reader_;
  }

private:
  std::optional<std::size_t> content_length_;
  std::optional<std::string_view> content_type_;

  http::HttpMethods method_;
  std::string host_;
  std::string url_;
  std::string version_;
  std::shared_ptr<HttpBodyStreamReader> stream_reader_;
  std::vector<char> buffer_;
  std::optional<std::string> path_;
};
} // namespace empty_d::http::request
