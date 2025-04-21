#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <string>
#include <utility>


using boost::asio::ip::tcp;

namespace empty_d { namespace http { namespace request {
class HttpBodyStreamReader {
public:

  explicit HttpBodyStreamReader(std::shared_ptr<boost::asio::steady_timer> bodyReadLock,
                                size_t body_content_legnt)
      : mBodyReadLock(std::move(bodyReadLock)),
        mBodyContentLength(body_content_legnt) {
    mBuffer.reserve(8096);
  }

  std::string text();
  void write(const std::string& data, boost::asio::yield_context yield);
  std::string readAny(boost::asio::yield_context yield);

  void setEof();

  [[nodiscard]] bool isEof() const { return mEof; }

private:
  size_t mBodyContentLength;
  std::shared_ptr<boost::asio::steady_timer> mBodyReadLock;
  bool mEof = false;
  std::string mBuffer{};
};
} } } // namespace empty_d::http::request
