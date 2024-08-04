#pragma once

#include "http/protocol/parser/http_request_parser.hpp"

namespace empty_d::http {
class BaseHttpMiddleware {
public:
  virtual HttpRequest &HandleRequest();
  
private:
};
} // namespace empty_d::http