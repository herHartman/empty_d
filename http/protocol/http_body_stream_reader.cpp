//
// Created by chopk on 12.04.24.
//

#include <iostream>
#include "http_body_stream_reader.h"

namespace http {

    awaitable<std::string> http_body_stream_reader::text() {
        co_await read_lock_->async_receive(boost::asio::use_awaitable);
        co_return std::string();
    }

    void http_body_stream_reader::write(const char* data) {
        string_buffer_ << data;
        read_lock_->try_send(boost::asio::error::eof);
    }

    void http_body_stream_reader::set_eof() {
        eof_ = true;
        read_lock_->try_send(boost::asio::error::eof);
    }

    awaitable<char *> http_body_stream_reader::read_any() {
        while (!string_buffer_.rdbuf()->in_avail() && !eof_) {
            co_await read_lock_->async_receive(boost::asio::use_awaitable);
        }
        std::string result;
        string_buffer_ >> result;
        char* res = const_cast<char *>(result.c_str());
        co_return res;
    }
}
