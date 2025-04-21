#include "http_headers.h"
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace empty_d::http {

void HttpHeaders::Set(const HeaderKey &key, HeaderValue value) {
  headers_.erase(key);
  std::vector<HeaderValue> values{std::move(value)};
  values.reserve(1);
  headers_[key] = values;
}

void HttpHeaders::Set(const HeaderKey &key, std::vector<HeaderValue> values) {
  headers_.erase(key);
  headers_[key] = std::move(values);
}

void HttpHeaders::Add(const HeaderKey &key, HeaderValue value) {
  auto entry = headers_.find(key);
  if (entry == headers_.end()) {
    Set(key, std::move(value));
  } else {
    entry->second.push_back(std::move(value));
  }
}

void HttpHeaders::Add(const HeaderKey &key, std::vector<HeaderValue> values) {
  auto entry = headers_.find(key);
  if (entry == headers_.end()) {
    Set(key, std::move(values));
  } else {
    for (const auto &iter : values) {
      entry->second.push_back(iter);
    }
  }
}

std::vector<HeaderValue> &HttpHeaders::GetHeaderValues(const HeaderKey &key) {
  return headers_[key];
}

size_t HttpHeaders::GetContentLength() {
  auto &content_length_vec = headers_[BasicHeaders::kContentLength];
  if (!content_length_vec.empty()) {
    auto &content_legnth_str = content_length_vec.back();
    return std::stoi(content_legnth_str);
  }
  return 0;
}

std::string &HttpHeaders::GetHost() {
  auto &host_vec = headers_[BasicHeaders::kHost];
  if (host_vec.empty()) {
    host_vec.emplace_back();
  }
  return host_vec.back();
}

std::string HttpHeaders::FormatHeaders() const {
  std::stringstream buf;
  for (const auto &header : headers_) {
  }
  buf << "\r\n";
  return buf.str();
}
} // namespace empty_d::http
