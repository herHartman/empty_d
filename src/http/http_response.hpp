#pragma once

#include "http/http_headers.h"
#include "http_status.h"

namespace empty_d::http {
class HttpResponse {
public:
  explicit HttpResponse(HttpStatus status) : status_(status){};

private:
  HttpHeaders headers_;
  std::string body_;
  HttpStatus status_;
  std::string reason_;
};
} // namespace empty_d::http