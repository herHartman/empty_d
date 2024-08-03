#pragma once

#include "http/http_headers.h"

namespace empty_d::http {
class HttpResponse {
public:
  HttpResponse() = default;

private:
  HttpHeaders headers_;
};
} // namespace empty_d::http