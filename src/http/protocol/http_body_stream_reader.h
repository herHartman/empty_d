#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

using boost::asio::awaitable;
using boost::asio::deferred;
using boost::asio::experimental::channel;
using boost::asio::ip::tcp;

namespace empty_d::http::request {
class HttpBodyStreamReader {
public:
  typedef channel<void(boost::system::error_code, std::size_t)> ReadLockChannel;

  typedef std::shared_ptr<ReadLockChannel> ReadLockChannel_P;

  explicit HttpBodyStreamReader(ReadLockChannel_P channel)
      : read_lock_(std::move(channel)) {
    buffer_.reserve(4096);
  }

  awaitable<std::string> Text();
  awaitable<void> Write(const char *data, std::size_t len);
  awaitable<std::vector<char>> ReadAny();

  void SetEof();

  [[nodiscard]] bool IsEof() const { return eof_; }

private:
  ReadLockChannel_P read_lock_;
  bool eof_ = false;
  std::stringstream string_buffer_;
  std::vector<char> buffer_;
};
} // namespace empty_d::http::request
