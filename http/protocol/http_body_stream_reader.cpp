//
// Created by chopk on 12.04.24.
//

#include <iostream>
#include "http_body_stream_reader.h"

namespace http {
    awaitable<std::string> http_body_stream_reader::text() {
        co_await read_lock_->async_receive(boost::asio::use_awaitable);
        std::cout << "receive result ok";
        co_return std::string();
    }
}
