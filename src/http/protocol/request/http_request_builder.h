#pragma once

#include "http/http_headers.h"
#include "http/protocol/http_methods.h"
#include "http/url_dispatcher.hpp"
#include "http_request.h"
#include <memory>
#include <optional>
#include <unordered_map>

namespace empty_d::http::request {

class HttpRequestBuilder {
public:
  explicit HttpRequestBuilder(std::shared_ptr<UrlDispatcher> url_dispatcher)
      : url_dispatcher_(std::move(url_dispatcher)) {}

  void AppendPath(const std::string &path);
  void AppendMethod(http::HttpMethods method);
  void AppendHttpVersion(std::string http_version);
  void AppendHeaders(HttpHeaders headers);
  void AppendHeaderField(std::string header_field);
  void AppendHeaderValue(std::string header_value);
  void AppendQuery(std::unordered_map<std::string, std::string> query);
  void AppendHeader(std::string, std::string);

  HttpRequest BuildRequest();

private:
  std::optional<std::string> path_;
  std::optional<std::string> http_version_;
  std::optional<HttpMethods> method_;
  std::unordered_map<std::string, std::string> query_;
  std::unordered_map<std::string, std::string> path_args_;
  HttpHeaders headers_;
  std::shared_ptr<UrlDispatcher> url_dispatcher_;
  std::optional<Resource> resource_;
};

}; // namespace empty_d::http::request
