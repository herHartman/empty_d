#pragma once

#include "http/protocol/http_methods.h"
#include "http/web/http_headers.h"
#include "http_request.h"
#include <optional>

namespace empty_d::http::request {

class HttpRequestBuilder {
public:
  void AppendPath(std::string path);
  void AppendMethod(http::HttpMethods method);
  void AppendHttpVersion(std::string http_version);
  void AppendHeaders(HttpHeaders headers);
  void AppendHeaderField(std::string header_field);
  void AppendHeaderValue(std::string header_value);
  void AppendQuery(std::unordered_map<std::string, std::string> query);
  void AppendHeader(std::string, std::string);

  HttpRequest BuildRequest();

private:
  std::optional<std::string> url_;
  std::optional<std::string> http_version_;
  std::optional<HttpMethods> method_;
  std::unordered_map<std::string, std::string> query;
  HttpHeaders headers_;
};

}; // namespace empty_d::http::request
