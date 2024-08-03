#include "http_body_stream_reader.h"
#include <cstring>

namespace empty_d::http::request {

awaitable<std::string> HttpBodyStreamReader::Text() { co_return std::string(); }

awaitable<void> HttpBodyStreamReader::Write(const char *data, std::size_t len) {
  if (buffer_.capacity() < (buffer_.size() + len)) {
    buffer_.reserve(2 * buffer_.capacity());
  }
  std::copy(data, data + len, std::back_inserter(buffer_));
  read_lock_->try_send(boost::system::error_code{}, 0);
  co_return;
}

void HttpBodyStreamReader::SetEof() {
  eof_ = true;
  read_lock_->try_send(boost::system::error_code{}, 0);
}

awaitable<std::string> HttpBodyStreamReader::ReadAny() {
  if (bucket_.empty()) {
    
  }

  while (buffer_.empty() && !eof_) {
    co_await read_lock_->async_receive(boost::asio::use_awaitable);
  }
  std::vector<char> result = std::move(buffer_);
  buffer_.reserve(4096);
  co_return std::move(result);
}
} // namespace empty_d::http::request
