#pragma once

#include <boost/asio.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>

using boost::asio::ip::tcp;

namespace empty_d::redis {
class BaseRedisConnection {
public:
  explicit BaseRedisConnection(boost::asio::io_context &io_context,
                               const std::string &address, std::size_t port)
      : io_context_(io_context), socket_(io_context),
        redis_server_endpoint_(boost::asio::ip::address::from_string(address),
                               port) {}

  [[nodiscard]] bool IsConnected() const;

  boost::asio::awaitable<void> Connect() {}

  void Disconnect();

private:
  boost::asio::io_context &io_context_;
  tcp::socket socket_;
  tcp::endpoint redis_server_endpoint_;

  boost::asio::awaitable<void> ConnectImpl() {
    socket_.open(tcp::v4());
    auto [error_code] = co_await socket_.async_connect(
        redis_server_endpoint_,
        boost::asio::as_tuple(boost::asio::use_awaitable));

  
  }
};
} // namespace empty_d::redis