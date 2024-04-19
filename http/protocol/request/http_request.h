//
// Created by chopk on 12.04.24.
//

#ifndef EMPTY_D_HTTP_REQUEST_H
#define EMPTY_D_HTTP_REQUEST_H

#include <string>
#include <boost/json.hpp>
#include "../../../core/boost_asio_types.h"
#include "../http_body_stream_reader.h"
#include "../../../io/serializable/json_serializable.h"

using namespace boost;

namespace http {
    class http_request {
    public:
        explicit http_request(std::shared_ptr<http_body_stream_reader> stream_reader)
            : stream_reader_(std::move(stream_reader)) {}

        template<is_serializable T>
        awaitable<T> read_body() {
            const char* data = co_await read();

        }

        awaitable<const char*> read() {
            if (buffer_.empty()) {
                while (true) {
                    buffer_.reserve(content_length_);
                    std::vector<char> chunk = co_await stream_reader_->read_any();
                    if (chunk.empty()) break;
                    buffer_.assign(chunk.begin(), chunk.end());
                }
            }
            co_return buffer_.data();
        }

    private:
        std::string host_;
        std::string url_;
        std::string version_;
        std::size_t content_length_{};
        std::string content_type_;
        std::shared_ptr<http_body_stream_reader> stream_reader_;
        std::vector<char> buffer_;
    };
}




#endif //EMPTY_D_HTTP_REQUEST_H
