
#include "http/http_server.h"
#include <boost/asio/awaitable.hpp>

namespace empty_d::http {
awaitable<void> HttpServer::Start() {
  is_running_ = true;
  while (is_running_) {
    ++requests_count_;
  }
}

void HttpServer::ShutDown() {
  acceptor_.close();
  is_running_ = false;
}
} // namespace empty_d::http
