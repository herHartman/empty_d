#pragma once

#include "http_server.h"
#include "url_dispatcher.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>

namespace empty_d::http {
class WebApplication {
public:
  WebApplication() = delete;
  WebApplication(std::string address, std::string port, size_t max_clients_size,
                 std::shared_ptr<UrlDispatcher> &url_dispatcher);

  void Run();

  void AddHandler(HttpHandler handler, HttpMethods method,
                  const std::string &path);

private:
  boost::asio::io_context io_context_;
  std::shared_ptr<UrlDispatcher> url_dispatcher_;
  std::unique_ptr<HttpServer> server_;
};
} // namespace empty_d::http