//
// Created by chopk on 12.04.24.
//

#include <cstring>
#include "http_body_stream_reader.h"

namespace http {

    awaitable<std::string> http_body_stream_reader::text() {
        co_return std::string();
    }

    awaitable<void> http_body_stream_reader::write(const char* data, std::size_t len) {
        if (buffer_.capacity() < (buffer_.size() + len)) {
            buffer_.reserve(2 * buffer_.capacity());
        }
        std::copy(data, data + len, std::back_inserter(buffer_));
        read_lock_->try_send(boost::system::error_code{}, 0);
        co_return;
    }

    void http_body_stream_reader::set_eof() {
        eof_ = true;
        read_lock_->try_send(boost::system::error_code{}, 0);
    }

    awaitable<std::vector<char>> http_body_stream_reader::read_any() {
        while (buffer_.empty() && !eof_) {
            co_await read_lock_->async_receive(boost::asio::use_awaitable);
        }
        std::vector<char> result = std::move(buffer_);
        buffer_.reserve(4096);
        co_return std::move(result);
    }
}
