//
// Created by chopk on 31.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_RAW_REQUEST_MESSAGE_H
#define SIMPLE_HTTP_SERVER_RAW_REQUEST_MESSAGE_H

#include <string>
#include <unordered_map>
#include "../http_methods.h"


namespace http {
    struct raw_request_message {
        http_methods method;
        std::string path;
        std::string version;
        std::unordered_map<std::string_view, std::string_view> headers;
        std::string compression;
        bool upgrade;
        bool chunked;
        std::string url;
    };
}



#endif //SIMPLE_HTTP_SERVER_RAW_REQUEST_MESSAGE_H
