//
// Created by chopk on 12.04.24.
//

#ifndef EMPTY_D_HTTP_BODY_STREAM_READER_H
#define EMPTY_D_HTTP_BODY_STREAM_READER_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <memory>
#include <utility>
#include <string>

using boost::asio::awaitable;
using boost::asio::ip::tcp;
using boost::asio::experimental::channel;
using boost::asio::deferred;

namespace http {
    class http_body_stream_reader {
    public:
        typedef std::shared_ptr<channel<void(boost::system::error_code, bool)>> read_lock_channel_p;

        explicit http_body_stream_reader(std::shared_ptr<channel<void(boost::system::error_code, bool)>> read_lock)
                : read_lock_(std::move(read_lock)) {}
        awaitable<std::string> text();
    private:
        read_lock_channel_p read_lock_;
    };
}


#endif //EMPTY_D_HTTP_BODY_STREAM_READER_H
