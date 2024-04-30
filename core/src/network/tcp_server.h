//
// Created by chopk on 19.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_TCP_SERVER_H
#define SIMPLE_HTTP_SERVER_TCP_SERVER_H


#include "http/web/http_protocol.h"
#include "transport.h"
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;
using boost::asio::detached;

class http_protocol_factory {
public:
  explicit http_protocol_factory(
      const std::shared_ptr<http::http_request_handler> &request_handler)
      : request_handler_(request_handler) {}

  base_protocol create_protocol(network::transport::transport_p transport) {
    return base_protocol(std::move(transport), request_handler_);
  }

private:
  std::shared_ptr<http::http_request_handler> request_handler_;
};

namespace network {
class tcp_server {
public:
  explicit tcp_server(boost::asio::io_context &io_context, const int port,
                      http_protocol_factory factory,
                      std::size_t max_requests = 1000)
      : acceptor_(io_context, tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        protocol_factory_(std::move(factory)), requests_count_(0),
        is_running_(false), max_requests_(max_requests) {}

  boost::asio::awaitable<void> start() {
    is_running_ = true;
    while (is_running_) {
      auto connection =
          protocol_factory_.create_protocol(std::make_shared<transport>(
              co_await acceptor_.async_accept(boost::asio::use_awaitable)));
      co_spawn(acceptor_.get_executor(),
               handle_connection(std::move(connection)), detached);

      ++requests_count_;
    }
  }

  void shutdown() {
    acceptor_.close();
    is_running_ = false;
  }

private:
  tcp::acceptor acceptor_;
  http_protocol_factory protocol_factory_;
  std::size_t requests_count_;
  bool is_running_;
  std::size_t max_requests_;

  awaitable<void> handle_connection(base_protocol protocol) {
    co_await protocol.handle();
    --requests_count_;
  }
};
} // namespace network

#endif // SIMPLE_HTTP_SERVER_TCP_SERVER_H
