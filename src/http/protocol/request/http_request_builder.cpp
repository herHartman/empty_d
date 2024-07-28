#include "http_request_builder.h"
#include "http/url_dispatcher.hpp"
#include <optional>

namespace empty_d::http::request {

void HttpRequestBuilder::AppendHeaderField(std::string header_field) {}

void HttpRequestBuilder::AppendHeaderValue(std::string header_value) {}

void HttpRequestBuilder::AppendHttpVersion(std::string http_version) {}

void HttpRequestBuilder::AppendMethod(http::HttpMethods method) {}

void HttpRequestBuilder::AppendPath(const std::string &path) {
  path_ = path;
  std::optional<Resource> resource = url_dispatcher_->GetResource(path);
  if (resource) {
    const auto &path_args = resource->GetPathArgs();
    for (auto &path_args : path_args) {
        
    }
  }
}

void HttpRequestBuilder::AppendQuery(
    std::unordered_map<std::string, std::string> query) {}

void HttpRequestBuilder::AppendHeader(std::string, std::string) {}

HttpRequest HttpRequestBuilder::BuildRequest() {}

} // namespace empty_d::http::request
