#include "http_request_builder.h"

namespace empty_d::http::request {

void HttpRequestBuilder::AppendHeaderField(std::string header_field) {}

void HttpRequestBuilder::AppendHeaderValue(std::string header_value) {}

void HttpRequestBuilder::AppendHttpVersion(std::string http_version) {}

void HttpRequestBuilder::AppendMethod(http::HttpMethods method) {}

void HttpRequestBuilder::AppendPath(std::string path) {}

} // namespace empty_d::http::request
