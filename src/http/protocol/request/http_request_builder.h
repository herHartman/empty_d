#pragma once

#include "http/protocol/http_methods.h"
#include "http/web/http_headers.h"
#include "http_request.h"
#include <optional>
#include <string_view>

namespace empty_d::http::request {

class HttpRequestBuilder {
public:
  void AppendPath(std::string path);
  void AppendMethod(http::HttpMethods method);
  void AppendHttpVersion(std::string_view http_version);
  void AppendHeaders(HttpHeaders headers);
  void AppendHeaderField(std::string header_field);
  void AppendHeaderValue(std::string header_value);

 HttpRequest BuildRequest();

private:
  std::optional<std::string> url_;
  std::optional<std::string> http_version_;
  std::optional<HttpMethods> method_;
  HttpHeaders headers_;
};

}; // namespace http
