//
// Created by chopk on 20.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_HTTP_REQUEST_PARSER_H
#define SIMPLE_HTTP_SERVER_HTTP_REQUEST_PARSER_H

#include <string>
#include <bits/stdc++.h>
#include <string_view>
#include <utility>
#include "raw_request_message.h"
#include "../http_methods.h"
#include "raw_request_message.h"
#include "../http_body_stream_reader.h"

constexpr static char HEADER_SEPARATOR = ':';
constexpr static char WHITESPACE = ' ';
constexpr static char LF = '\n';
constexpr static char CR = '\r';

namespace http {

    enum class request_state {
        PARSE_METHOD,
        PARSE_PATH,
        PARSE_HTTP_VERSION,
        PARSE_HEADERS,
        PARSE_COMPLETE,
        PARSE_HEADERS_ALMOST_DONE
    };

    static std::unordered_map<std::string_view, http_methods> http_methods_map = {
        {"OPTIONS", http_methods::HTTP_OPTIONS},
        {"GET", http_methods::HTTP_GET},
        {"HEAD", http_methods::HTTP_HEAD},
        {"POST", http_methods::HTTP_POST},
        {"PUT", http::HTTP_PUT},
        {"PATCH", http_methods::HTTP_PATCH},
        {"DELETE", http_methods::HTTP_DELETE},
        {"TRACE", http_methods::HTTP_TRACE},
        {"CONNECT", http_methods::HTTP_CONNECT}
    };


    class http_headers_parser {
    public:

        enum class parse_header_state {
            PARSE_NAME,
            PARSE_VALUE
        };

        void parse_headers(
            const std::string_view& data,
            raw_request_message& request_message,
            const std::size_t start_position,
            const std::size_t len,
            request_state& request_state
        ) {
            std::size_t current_position = start_position;
            std::size_t current_substr_pos = start_position;
            while (current_position < len) {
                switch (current_state_) {
                    case parse_header_state::PARSE_NAME:
                        if (data[current_position] == HEADER_SEPARATOR) {
                            current_header_name_ = data.substr(
                                current_substr_pos,
                                current_position - current_substr_pos
                            );
                            current_state_ = parse_header_state::PARSE_VALUE;
                            current_substr_pos = current_position;
                        } else if (data[current_position] == CR || data[current_position] == LF) {
                            request_state = request_state::PARSE_HEADERS_ALMOST_DONE;
                            return;
                        }
                        break;
                    case parse_header_state::PARSE_VALUE:
                        if (data[current_position] == WHITESPACE) {
                            ++current_substr_pos;
                        } else if (data[current_position] == CR) {
                            const std::string_view header_value = data.substr(
                                current_substr_pos,
                                current_position - current_substr_pos
                            );
                            request_message.headers[current_header_name_] = header_value;

                        } else if (data[current_position] == LF) {
                            current_substr_pos = current_position + 1;
                            current_state_ = parse_header_state::PARSE_NAME;
                        }
                        break;
                }
                ++current_position;
            }
        }
    private:
        parse_header_state current_state_;
        std::string_view current_header_name_;

    };

    class http_request_parser {
    public:

        explicit http_request_parser()
         :  headers_parser_() {}

        [[nodiscard]] std::size_t parse_message(
            const char* data_,
            const std::size_t len,
            raw_request_message& request_message
        ) {
            std::string_view data(data_);
            std::size_t current_substr_start_pos = 0;

            for (int i = 0; i < len; ++i) {
                switch (current_state_) {
                    case request_state::PARSE_METHOD:
                        if (data[i] == WHITESPACE) {
                            current_state_ = request_state::PARSE_PATH;
                            request_message.method = http_methods_map[
                                data.substr(current_substr_start_pos, i - current_substr_start_pos)
                            ];
                            current_substr_start_pos = i + 1;
                        }
                        break;
                    case request_state::PARSE_PATH:
                        if (data[i] == WHITESPACE) {
                            request_message.path = data.substr(current_substr_start_pos, i - current_substr_start_pos);
                            current_state_ = request_state::PARSE_HTTP_VERSION;
                            current_substr_start_pos = i + 1;
                        }
                        break;
                    case request_state::PARSE_HTTP_VERSION:
                        if (data[i] == CR) {
                            request_message.version = data.substr(
                                current_substr_start_pos,
                                i - current_substr_start_pos
                            );
                            current_state_ = request_state::PARSE_HEADERS;
                        }
                        break;
                    case request_state::PARSE_HEADERS:
                        if (data[i] == LF) continue;
                        headers_parser_.parse_headers(data, request_message, i, len, current_state_);
                        current_state_ = request_state::PARSE_HEADERS_ALMOST_DONE;
                        break;
                    case request_state::PARSE_COMPLETE:
                        return 0;
                    case request_state::PARSE_HEADERS_ALMOST_DONE:
                        // current_state_ = request_state::PARSE_COMPLETE;
                        return i;
                }
            }
        }

        void parse_body(const char* data, const std::size_t len, raw_request_message& request_message) {

        }

        [[nodiscard]] request_state get_state() const {
            return current_state_;
        }

        [[nodiscard]] bool is_parse_message_complete() const {
            return current_state_ == request_state::PARSE_HEADERS_ALMOST_DONE;
        }

        void set_payload(std::shared_ptr<http::http_body_stream_reader> payload) {
            payload_ = std::move(payload);
        }

    private:
        request_state current_state_ = request_state::PARSE_METHOD;
        http_headers_parser headers_parser_;
        std::shared_ptr<http::http_body_stream_reader> payload_ = nullptr;
    };
}



#endif //SIMPLE_HTTP_SERVER_HTTP_REQUEST_PARSER_H
