#pragma once

#include "definitions/boost_asio_types.h"
#include "http/http_headers.h"
#include "http/protocol/http_body_stream_reader.h"
#include "http/protocol/http_methods.h"
#include "io/serializable/json_serializable.h"
#include <boost/json.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace boost;

namespace empty_d::http::request {
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
      : content_length_(content_length), method_(method_),
        headers_(std::move(headers)), host_(std::move(host)),
        http_version_(std::move(http_version)), stream_reader_(std::move(body)),
        path_(std::move(path)), query_args_(std::move(query_args)),
        path_args_(std::move(path_args)), buffer_{} {}

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
        std::string chunk = co_await stream_reader_->ReadAny();
        if (chunk.empty())
          break;
        buffer_.assign(chunk.begin(), chunk.end());
      }
    }
    co_return buffer_.data();
  }

  [[nodiscard]] std::size_t GetContentLength() const { return content_length_; }

  [[nodiscard]] const std::string &GetPath() const { return path_; }

  [[nodiscard]] http::HttpMethods GetMethod() const { return method_; }

  [[nodiscard]] std::shared_ptr<HttpBodyStreamReader> GetStreamReader() const {
    return stream_reader_;
  }

  const std::vector<std::string> &GetHeaders(std::string_view key);

private:
  std::size_t content_length_;

  HttpMethods method_;
  HttpHeaders headers_;
  std::string host_;
  std::string http_version_;
  std::shared_ptr<HttpBodyStreamReader> stream_reader_;
  std::vector<char> buffer_;
  std::string path_;
  std::unordered_map<std::string, std::string> query_args_;
  std::unordered_map<std::string, std::string> path_args_;
};
} // namespace empty_d::http::request
