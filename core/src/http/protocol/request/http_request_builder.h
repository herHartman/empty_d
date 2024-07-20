#pragma once

#include "http/protocol/http_methods.h"
#include "http/web/http_headers.h"
#include "http_request.h"
#include <optional>
#include <string_view>

namespace http {

class HttpRequestBuilder {
public:
  void AppendPath(std::string url);
  void AppendMethod(http::http_methods method);
  void AppendHttpVersion(std::string_view http_version);
  void AppendHeaders(web::headers headers);

 Request build_request();

private:
  std::optional<std::string_view> url_;
  std::optional<std::string_view> http_version_;
  std::optional<http_methods> method_;
  web::headers headers_;
};

}; // namespace http
