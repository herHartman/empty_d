#pragma once

#include "http/protocol/http_methods.h"
#include "http/web/http_headers.h"
#include "http_request.h"
#include <optional>
#include <string_view>

namespace http {

class request_builder {
public:
  request_builder() = default;
  ~request_builder() = default;
  void append_path(std::string_view url);
  void append_method(http::http_methods method);
  void append_http_version(std::string_view http_version);
  void append_headers(web::headers headers);
  
  request build_request();

private:
  std::optional<std::string_view> url_;
  std::optional<std::string_view> http_version_;
  std::optional<http_methods> method_;
  web::headers headers_;
};

}; // namespace http
