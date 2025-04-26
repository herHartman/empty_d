#include "http_body_stream_reader.h"

namespace empty_d::http::request {

std::string HttpBodyStreamReader::text() { return {}; }

void HttpBodyStreamReader::write(const std::string& data, boost::asio::yield_context yield) {
  if (mBuffer.capacity() < (mBuffer.size() + data.length())) {
    mBuffer.reserve(2 * mBuffer.capacity());
  }
  mBuffer.append(data);
  mBodyReadLock->cancel_one();
}

void HttpBodyStreamReader::setEof() {
  mEof = true;
  mBodyReadLock->cancel_one();
}

std::string HttpBodyStreamReader::readAny(boost::asio::yield_context yield) {
  while (mBuffer.empty() && !mEof) {
    mBodyReadLock->async_wait(yield);
  }
  
  std::string result = std::move(mBuffer);
  mBuffer.clear();
  return result;
}
}   // namespace empty_d::http::request
