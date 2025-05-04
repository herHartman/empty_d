#include "http_body_stream_reader.h"
#include <boost/asio/buffers_iterator.hpp>
#include <stdexcept>

namespace empty_d::http::request {

size_t HttpBodyStreamReader::bytesRemaining() const {
  return mBodyContentLength - mTotalWritten;
}

bool HttpBodyStreamReader::isWriteComplete() const {
  return mBodyContentLength == mTotalWritten;
}

std::string HttpBodyStreamReader::text() { return {}; }

void HttpBodyStreamReader::setEof() {
  mEof = true;
}

std::string HttpBodyStreamReader::readAny(boost::asio::yield_context yield) {
  if (mEof) {
    throw std::runtime_error("stream eof");
  }

  while (mBuffer.size() == 0) {
    mBodyReadLock.async_wait(yield);
  }

  std::string result{boost::asio::buffers_begin(mBuffer.data()),
                     boost::asio::buffers_end(mBuffer.data())};

  mBuffer.consume(result.size());
  mDataReadSize += result.size();
  if (mDataReadSize == mBodyContentLength) {
    setEof();
  }
  return result;
}
} // namespace empty_d::http::request
