//
// Created by chopk on 12.04.24.
//

#ifndef EMPTY_D_HTTP_REQUEST_H
#define EMPTY_D_HTTP_REQUEST_H

#include <string>

namespace http {
    class http_request {
    public:
    private:
        std::string host_;
        std::string url_;
        std::string version_;
        std::size_t content_length_;
        std::string content_type_;
    };
}




#endif //EMPTY_D_HTTP_REQUEST_H
