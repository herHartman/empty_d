#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/experimental/channel.hpp>

#include <memory>
#include <utility>

using boost::asio::awaitable;
using boost::asio::deferred;
using boost::asio::experimental::channel;
using boost::asio::ip::tcp;
using namespace boost::asio::experimental::awaitable_operators;

namespace empty_d::network {
class Transport {
public:
  typedef std::shared_ptr<Transport> Transport_P;

  explicit Transport(tcp::socket socket) : socket_(std::move(socket)) {}

  template <typename MutableBufferSequence>
  awaitable<std::size_t> Read(const MutableBufferSequence &buffers) {
    std::size_t len =
        co_await socket_.async_read_some(buffers, boost::asio::use_awaitable);
    co_return len;
  }

  template <typename ConstBufferSequence>
  awaitable<void> Write(const ConstBufferSequence &buffers) {
    co_await socket_.async_write_some(buffers,
                                      as_tuple(boost::asio::use_awaitable));
  }

  [[nodiscard]] auto GetExecutor() { return socket_.get_executor(); }

  [[nodiscard]] bool IsOpen() const { return socket_.is_open(); }

  void Close() {
    socket_.shutdown(tcp::socket::shutdown_send);
    socket_.shutdown(tcp::socket::shutdown_receive);
    socket_.close();
  }

private:
  tcp::socket socket_;
  std::string bucket_{};
  boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                     std::allocator<char>>
      read_buffer_{bucket_};
};

} // namespace empty_d::network
