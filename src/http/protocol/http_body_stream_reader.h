#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <memory>
#include <string>
#include <utility>

using boost::asio::awaitable;
using boost::asio::deferred;
using boost::asio::experimental::channel;
using boost::asio::ip::tcp;

namespace empty_d::http::request {
class HttpBodyStreamReader {
public:
  template <typename AsyncReadStream, typename DynamicBuffer> class ReadAnyOp;

  typedef channel<void(boost::system::error_code, std::size_t)> ReadLockChannel;

  typedef std::shared_ptr<ReadLockChannel> ReadLockChannel_P;

  explicit HttpBodyStreamReader(ReadLockChannel_P channel,
                                size_t body_content_legnt)
      : read_lock_(std::move(channel)),
        body_content_length(body_content_legnt) {
    buffer_.reserve(8096);
  }

  awaitable<std::string> Text();
  awaitable<void> Write(std::string data);
  awaitable<std::string> ReadAny();

  void SetEof();

  [[nodiscard]] bool IsEof() const { return eof_; }

private:
  size_t body_content_length;
  ReadLockChannel_P read_lock_;
  bool eof_ = false;
  std::string buffer_{};
};
} // namespace empty_d::http::request
