#pragma once

#include <boost/asio.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <iterator>
#include <string>

using boost::asio::ip::tcp;

namespace empty_d::http::request {
class HttpBodyStreamReader {
public:
  HttpBodyStreamReader(const boost::asio::any_io_executor &executor,
                       size_t body_content_legnt)
      : mBodyReadLock(executor), mBodyContentLength(body_content_legnt),
        mDataReadSize(0) {}

  size_t bytesRemaining() const;
  bool isWriteComplete() const;
  std::string text();

  template <typename Iterator> void write(Iterator begin, Iterator end);

  std::string readAny(boost::asio::yield_context yield);

  void setEof();

  [[nodiscard]] bool isEof() const { return mEof; }

private:
  boost::asio::streambuf mBuffer;
  size_t mBodyContentLength;
  boost::asio::steady_timer mBodyReadLock;
  bool mEof = false;
  size_t mDataReadSize;
  size_t mTotalWritten{};
  // std::string mBuffer{};
};

template <typename Iterator>
void HttpBodyStreamReader::write(Iterator begin, Iterator end) {
  const size_t addSize = std::distance(begin, end);

  const size_t writeSize = std::min(addSize, bytesRemaining());

  boost::asio::buffer_copy(mBuffer.prepare(addSize),
                           boost::asio::buffer(&*begin, addSize));

  mBuffer.commit(addSize);
  mTotalWritten += writeSize;
  mBodyReadLock.cancel();
}

} // namespace empty_d::http::request
