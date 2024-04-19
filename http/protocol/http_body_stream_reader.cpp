//
// Created by chopk on 12.04.24.
//

#include <iostream>
#include "http_body_stream_reader.h"

namespace http {

    awaitable<std::string> http_body_stream_reader::text() {
        co_return std::string();
    }

    awaitable<void> http_body_stream_reader::write(const char* data) {
        string_buffer_ << data;
        read_lock_->try_send(boost::system::error_code{}, 0);
        co_return;
    }

    void http_body_stream_reader::set_eof() {
        eof_ = true;
        read_lock_->try_send(boost::system::error_code{}, 0);
    }

    awaitable<std::vector<char>> http_body_stream_reader::read_any() {
        while ((string_buffer_.peek() == -1) && !eof_) {
            string_buffer_.clear();
            std::size_t bytes_available = co_await read_lock_->async_receive(boost::asio::use_awaitable);
        }
        std::string result;
        string_buffer_ >> result;
        std::vector<char> res;
        res.reserve(result.size());
        co_return res;
    }
}
