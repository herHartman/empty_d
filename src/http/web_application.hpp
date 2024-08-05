#pragma once

#include "http_server.h"
#include "url_dispatcher.hpp"
#include <memory>

namespace empty_d::http {
class WebApplication {
public:
private:
  std::unique_ptr<HttpServer> server_;
  std::shared_ptr<UrlDispatcher> url_dispatcher_;
};
} // namespace empty_d::http