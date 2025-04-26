#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;

namespace empty_d::network {
class Transport {
public:
  explicit Transport(tcp::socket socket)
      : mSocket(std::move(socket)) {}

  using executor_type = tcp::socket::executor_type;
  
  template <typename MutableBufferSequence>
  size_t asyncRead(const MutableBufferSequence &buffers, boost::asio::yield_context yield) {
    return mSocket.async_read_some(buffers, yield);
  }

  template <typename ConstBufferSequence>
  void asyncWrite(const ConstBufferSequence &buffers, boost::asio::yield_context yield) {
    mSocket.async_write_some(buffers, yield);
  }

  executor_type getExecutor() { return mSocket.get_executor(); }

  [[nodiscard]] bool isOpen() const { return mSocket.is_open(); }

  void Close() {
    mSocket.shutdown(tcp::socket::shutdown_send);
    mSocket.shutdown(tcp::socket::shutdown_receive);
    mSocket.close();
  }

private:
  tcp::socket mSocket;
  std::string mBucket{};
  boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                     std::allocator<char>>
      mReadBuffer{mBucket};
};

} // namespace empty_d::network

