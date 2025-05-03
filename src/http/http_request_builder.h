#pragma once

#include "http/http_headers.h"
#include "http/http_body_stream_reader.h"
#include "http/http_methods.h"
#include "http/url_dispatcher.hpp"
#include <boost/optional.hpp>
#include "http_request.h"
#include <memory>
#include <unordered_map>

namespace empty_d::http::request {

class HttpRequestBuilder {
public:
  explicit HttpRequestBuilder(std::shared_ptr<UrlDispatcher> url_dispatcher)
      : url_dispatcher_{std::move(url_dispatcher)}, body_reader_{nullptr} {}

  void AppendPath(const std::string &path);
  void AppendMethod(http::HttpMethods method);
  void AppendHttpVersion(const std::string &http_version);
  void AppendHeaders(HttpHeaders headers);
  void AppendHeaderField(const std::string &header_field);
  void AppendHeaderValue(const std::string &header_value);
  void AppendQuery(const std::unordered_map<std::string, std::string> &query);
  void AppendHeader(const std::string &header_field,
                    const std::string &header_value);
  void AppendBody(const std::string &body);

  std::pair<HttpRequest, HttpHandler> BuildRequest();

  boost::optional<Resource> GetResource() const;

private:
  boost::optional<std::string> path_;
  boost::optional<std::string> http_version_;
  boost::optional<HttpMethods> method_;
  std::unordered_map<std::string, std::string> query_;
  std::unordered_map<std::string, std::string> path_args_;
  HttpHeaders headers_;
  std::shared_ptr<UrlDispatcher> url_dispatcher_;
  boost::optional<Resource> resource_;
  std::shared_ptr<request::HttpBodyStreamReader> body_reader_;
  boost::optional<std::string> current_header_field_;
};

}  ; // namespace empty_d::http::request
