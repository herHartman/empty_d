#pragma once

#include "http/protocol/http_connection.h"
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>


using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;
using boost::asio::detached;

namespace empty_d::http {
class HttpServer {
public:
  explicit HttpServer(boost::asio::io_context &io_context, const int port,
                      std::size_t max_requests = 1000)
      : acceptor_(io_context, tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        requests_count_(0), is_running_(false), max_requests_(max_requests) {}

  boost::asio::awaitable<void> Start();
  void ShutDown();

private:
  tcp::acceptor acceptor_;
  std::size_t requests_count_;
  bool is_running_;
  std::size_t max_requests_;

  awaitable<void> HandleConnection(HttpConnection handler) {
    co_await handler.Handle();
    --requests_count_;
  }
};
} // namespace empty_d::http
