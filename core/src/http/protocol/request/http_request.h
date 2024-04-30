//
// Created by chopk on 12.04.24.
//

#ifndef EMPTY_D_HTTP_REQUEST_H
#define EMPTY_D_HTTP_REQUEST_H

#include "definitions/boost_asio_types.h"
#include "http/protocol/http_body_stream_reader.h"
#include "http/protocol/request/raw_request_message.h"
#include "io/serializable/json_serializable.h"
#include <string>
#include <boost/json.hpp>

using namespace boost;

namespace http {
    class http_request {
    public:
        explicit http_request(
                std::shared_ptr<http_body_stream_reader> stream_reader,
                raw_request_message request_message
        )
            : stream_reader_(std::move(stream_reader)), request_message_(std::move(request_message))  {}

        template<is_serializable T>
        awaitable<T> read_body() {
            co_return typename T::serializer{}.deserialize(co_await read_json());
        }

        awaitable<json::value> read_json() {
            const char* data = co_await read();
            boost::system::error_code ec;
            json::value json_value = json::parse(data, ec);
            co_return json_value;
        }

        awaitable<const char*> read() {
            if (buffer_.empty()) {
                while (true) {
                    buffer_.reserve(get_content_length());
                    std::vector<char> chunk = co_await stream_reader_->read_any();
                    if (chunk.empty()) break;
                    buffer_.assign(chunk.begin(), chunk.end());
                }
            }
            co_return buffer_.data();
        }

    [[nodiscard]] std::size_t get_content_length() {
        if (!content_length_) {
            content_length_ = request_message_.get_content_length();
        }
        return content_length_.value();
    }

    [[nodiscard]] std::string get_path() {
        if (!path_) {
            path_ = request_message_.path;
        }
        return path_.value();
    }

    [[nodiscard]] http::http_methods get_method() const {
        return request_message_.method;
    }

    [[nodiscard]] std::shared_ptr<http_body_stream_reader> get_stream_reader() const {
        return stream_reader_;
    }

    private:
        std::optional<std::size_t> content_length_;
        std::optional<std::string_view> content_type_;

        std::string host_;
        std::string url_;
        std::string version_;
        std::shared_ptr<http_body_stream_reader> stream_reader_;
        std::vector<char> buffer_;
        std::optional<std::string> path_;
        raw_request_message request_message_;
    };
}




#endif //EMPTY_D_HTTP_REQUEST_H
