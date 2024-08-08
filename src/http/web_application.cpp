#include "web_application.hpp"
#include "http/http_server.h"
#include "http/protocol/http_methods.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <utility>

namespace empty_d::http {

WebApplication::WebApplication(std::string address, std::string port,
                               size_t max_clients_size,
                               std::shared_ptr<UrlDispatcher> &url_dispatcher)
    : url_dispatcher_(url_dispatcher),
      server_(new HttpServer(io_context_, std::move(address), std::move(port),
                             url_dispatcher, max_clients_size)) {}

void WebApplication::Run() {
  boost::asio::co_spawn(io_context_, server_->Start(boost::asio::use_awaitable),
                        boost::asio::detached);
  io_context_.run();
}

void WebApplication::AddHandler(HttpHandler handler, HttpMethods method,
                                const std::string &path) {
  url_dispatcher_->AddHandler(handler, method, path);
}

} // namespace empty_d::http