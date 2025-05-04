#pragma once

#include <boost/asio.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

namespace empty_d::http::request {
class HttpBodyStreamReader {
public:
  HttpBodyStreamReader(const boost::asio::any_io_executor &executor,
                       size_t body_content_legnt)
      : mBodyReadLock(executor), mBodyContentLength(body_content_legnt), mDataReadSize(0) {
    mBuffer.reserve(8096);
  }

  HttpBodyStreamReader(const boost::asio::any_io_executor &executor,
                       size_t body_content_legnt, std::string body)
      : mBodyReadLock(executor), mBodyContentLength(body_content_legnt),
        mBuffer(std::move(body)), mDataReadSize(0) {
    mBuffer.reserve(8096);
  }

  std::string text();
  void write(const std::string &data, boost::asio::yield_context yield);
  std::string readAny(boost::asio::yield_context yield);

  void setEof();

  [[nodiscard]] bool isEof() const { return mEof; }

  ~HttpBodyStreamReader() {
    std::cout << __PRETTY_FUNCTION__ << ": try destroy " << std::endl;
  }

private:
  size_t mBodyContentLength;
  boost::asio::steady_timer mBodyReadLock;
  bool mEof = false;
  size_t mDataReadSize;
  std::string mBuffer{};
};
} // namespace empty_d::http::request
