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
                                std::shared_ptr<tcp::socket> socket,
                                size_t body_content_legnt)
      : read_lock_(std::move(channel)), socket_(std::move(socket)),
        body_content_length(body_content_legnt) {
    bucket_.reserve(8096);
  }

  awaitable<std::string> Text();
  awaitable<void> Write(const char *data, std::size_t len);
  awaitable<std::string> ReadAny();

  void SetEof();

  [[nodiscard]] bool IsEof() const { return eof_; }

private:
  using ReadBuffer =
      boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                         std::allocator<char>>;
  size_t body_content_length;
  std::shared_ptr<tcp::socket> socket_;
  ReadLockChannel_P read_lock_;
  bool eof_ = false;
  std::string bucket_{};
  ReadBuffer buffer_{bucket_};
};

template <typename AsyncReadStream, typename DynamicBuffer>
class HttpBodyStreamReader::ReadAnyOp {
private:
  AsyncReadStream &stream_;
  boost::asio::coroutine coro_{};
  DynamicBuffer buf_;
  size_t len_;

  ReadAnyOp(AsyncReadStream &stream, DynamicBuffer buf, size_t size)
      : stream_(stream), buf_(std::move(buf)), len_(len_) {}

  template <class Self>
  void operator()(Self &self, boost::system::error_code ec = {},
                  std::size_t n = 0) {

    BOOST_ASIO_CORO_REENTER(coro_) {
      // BOOST_ASIO_CORO_YIELD
      // stream_.async_read_some( std::move(self))
    }
  }
};

} // namespace empty_d::http::request
