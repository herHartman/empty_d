//
// Created by chopk on 20.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_HTTP_REQUEST_PARSER_H
#define SIMPLE_HTTP_SERVER_HTTP_REQUEST_PARSER_H

#include "http/protocol/http_body_stream_reader.h"
#include "http/protocol/request/http_request.h"
#include "http/protocol/request/request_builder.h"
#include "http/protocol/url.h"
#include "http/web/http_headers.h"
#include <bits/stdc++.h>
#include <string_view>
#include <tuple>
#include <utility>

constexpr static char HEADER_SEPARATOR = ':';
constexpr static char WHITESPACE = ' ';
constexpr static char LF = '\n';
constexpr static char CR = '\r';

namespace http {

class headers_parser {
public:
  typedef std::tuple<std::optional<web::headers>, int> parse_result;
  enum class parse_header_state {
    PARSE_HEADER_FIELD_START,
    PARSE_NAME,
    PARSE_VALUE,
    PARSE_SPACES_BEFORE_VALUE
  };

  headers_parser() : current_state_(parse_header_state::PARSE_NAME) {}
  headers_parser(headers_parser &parser) = default;
  headers_parser(headers_parser &&parser) = default;
  ~headers_parser() = default;

  parse_result parse_headers(const char *data, std::size_t len,
                             std::size_t *body_start_index);

private:
  parse_header_state current_state_;
  std::string_view current_header_name_;
  web::headers result_;
};

class url_parser {
public:
  url_parser() = default;
  url_parser(url_parser &parser) = default;
  url_parser(url_parser &&parser) = default;

  ~url_parser() = default;

  url_utils::url parser_url(const char *data, std::size_t len);

private:
  url_utils::url_builder url_builder_;
};

class http_request_parser {
public:
  explicit http_request_parser() : headers_parser_() {}

  Request parse_message(const char *data, std::size_t len,
                        std::size_t *body_start_position);

  awaitable<void> parse_body(const char *data, const std::size_t len,
                             raw_request_message &request_message) {
    if (payload_) {
      current_body_length_ += len;
      co_await payload_->write(data, len);
      if (current_body_length_ == request_message.get_content_length()) {
        payload_->set_eof();
      }
    }
  }

  void set_payload(std::shared_ptr<http::http_body_stream_reader> payload) {
    payload_ = std::move(payload);
  }

private:
  enum class request_state {
    PARSE_METHOD = 1,
    PARSE_PATH,
    PARSE_HTTP_VERSION,
    PARSE_HEADERS,
  };

  request_state current_state_ = request_state::PARSE_METHOD;
  headers_parser headers_parser_;
  std::shared_ptr<http::http_body_stream_reader> payload_ = nullptr;
  HttpRequestBuilder request_builder_;
  std::size_t current_body_length_ = 0;
  std::size_t last_data_position_ = 0;
};
} // namespace http

#endif // SIMPLE_HTTP_SERVER_HTTP_REQUEST_PARSER_H
