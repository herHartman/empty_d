#include "http_request_builder.h"

namespace empty_d::http::request {

void HttpRequestBuilder::AppendHeaderField(std::string header_field) {}

void HttpRequestBuilder::AppendHeaderValue(std::string header_value) {}

void HttpRequestBuilder::AppendHttpVersion(std::string http_version) {}

void HttpRequestBuilder::AppendMethod(http::HttpMethods method) {}

void HttpRequestBuilder::AppendPath(std::string path) {}

void HttpRequestBuilder::AppendQuery(
    std::unordered_map<std::string, std::string> query) {}

void HttpRequestBuilder::AppendHeader(std::string, std::string) {}

HttpRequest HttpRequestBuilder::BuildRequest() {}

} // namespace empty_d::http::request
