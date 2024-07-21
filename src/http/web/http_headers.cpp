//
// Created by chopk on 11.04.24.
//

#include "http_headers.h"
#include <sstream>
#include <utility>
#include <vector>

namespace empty_d::http {

void HttpHeaders::Set(HeaderKey key, HeaderValue value) {
  headers_.erase(key);
  std::vector<HeaderKey> values{std::move(value)};
  values.reserve(1);
  headers_.insert({std::move(key), std::move(values)});
}

void HttpHeaders::Set(HeaderKey key, std::vector<HeaderValue> values) {
  headers_.erase(key);
  headers_.insert({std::move(key), std::move(values)});
}

void HttpHeaders::Add(HeaderKey key, HeaderValue value) {
  auto entry = headers_.find(key);
  if (entry == headers_.end()) {
    Set(std::move(key), std::move(value));
  } else {
    entry->second.push_back(std::move(value));
  }
}

void HttpHeaders::Add(HeaderKey key, std::vector<HeaderValue> values) {
  auto entry = headers_.find(key);
  if (entry == headers_.end()) {
    Set(std::move(key), std::move(values));
  } else {
    for (const auto &iter : values) {
      entry->second.push_back(iter);
    }
  }
}

std::string HttpHeaders::FormatHeaders() const {
  std::stringstream buf;
  for (const auto &header : headers_) {
  }
  buf << "\r\n";
  return buf.str();
}
} // namespace empty_d::http
