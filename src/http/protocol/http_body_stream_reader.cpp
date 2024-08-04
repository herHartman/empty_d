#include "http_body_stream_reader.h"
#include <cstring>

namespace empty_d::http::request {

awaitable<std::string> HttpBodyStreamReader::Text() { co_return std::string(); }

awaitable<void> HttpBodyStreamReader::Write(std::string data) {
  if (buffer_.capacity() < (buffer_.size() + data.length())) {
    buffer_.reserve(2 * buffer_.capacity());
  }
  buffer_.append(data);
  read_lock_->try_send(boost::system::error_code{}, 0);
  co_return;
}

void HttpBodyStreamReader::SetEof() {
  eof_ = true;
  read_lock_->try_send(boost::system::error_code{}, 0);
}

awaitable<std::string> HttpBodyStreamReader::ReadAny() {
  while (buffer_.empty() && !eof_) {
    co_await read_lock_->async_receive(boost::asio::use_awaitable);
  }
  std::string result = std::move(buffer_);
  co_return std::move(result);
}
} // namespace empty_d::http::request
