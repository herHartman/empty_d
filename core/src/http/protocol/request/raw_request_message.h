//
// Created by chopk on 31.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_RAW_REQUEST_MESSAGE_H
#define SIMPLE_HTTP_SERVER_RAW_REQUEST_MESSAGE_H

#include <string>
#include <unordered_map>
#include <charconv>
#include <string_view>
#include <vector>
#include "http/protocol/http_methods.h"

namespace http {
    struct raw_request_message {
        http_methods method;
        std::string path;
        std::string version;
        std::unordered_map<std::string_view, std::vector<std::string_view>> headers;
        std::string compression;
        bool upgrade;
        bool chunked;
        std::string url;

        std::size_t get_content_length() const {
            int content_length = 0;
            if (headers.contains("Content-Length")) {
                // std::string_view content_length_str = headers.at("Content-Length");
                // auto result = std::from_chars(
                //         content_length_str.data(),
                //         content_length_str.data() + content_length_str.size(),
                //         content_length
                // );
                // if (result.ec == std::errc::invalid_argument) {
                //     throw std::exception();
                // }
            }
            return content_length;
        }

        bool has_body() const {
            return get_content_length() > 0;
        }
    };
}



#endif //SIMPLE_HTTP_SERVER_RAW_REQUEST_MESSAGE_H
