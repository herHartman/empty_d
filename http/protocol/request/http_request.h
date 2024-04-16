//
// Created by chopk on 12.04.24.
//

#ifndef EMPTY_D_HTTP_REQUEST_H
#define EMPTY_D_HTTP_REQUEST_H

#include <string>
#
#include "../http_body_stream_reader.h"

namespace http {
    class http_request {
    public:
        explicit http_request(std::shared_ptr<http_body_stream_reader> stream_reader)
            : stream_reader_(std::move(stream_reader)) {}
    private:
        std::string host_;
        std::string url_;
        std::string version_;
        std::size_t content_length_;
        std::string content_type_;

        std::shared_ptr<http_body_stream_reader> stream_reader_;
    };
}




#endif //EMPTY_D_HTTP_REQUEST_H
