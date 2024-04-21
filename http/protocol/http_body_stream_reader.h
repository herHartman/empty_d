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
#include <sstream>


using boost::asio::awaitable;
using boost::asio::ip::tcp;
using boost::asio::experimental::channel;
using boost::asio::deferred;

namespace http {
    class http_body_stream_reader {
    public:
        typedef channel<void(boost::system::error_code, std::size_t)> read_lock_channel;
        typedef std::shared_ptr<read_lock_channel> read_lock_channel_p;

        explicit http_body_stream_reader(read_lock_channel_p channel) : read_lock_(std::move(channel)) {
            buffer_.reserve(4096);
        }

        awaitable<std::string> text();
        awaitable<void> write(const char* data, std::size_t len);
        awaitable<std::vector<char>> read_any();

        void set_eof();

        [[nodiscard]] bool is_eof() const  { return eof_; }

    private:
        read_lock_channel_p read_lock_;
        bool eof_ = false;
        std::stringstream string_buffer_;
        std::vector<char> buffer_;
    };
}


#endif //EMPTY_D_HTTP_BODY_STREAM_READER_H
