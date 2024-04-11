//
// Created by chopk on 10.04.24.
//

#ifndef TEST_HTTP_RESPONSE_H
#define TEST_HTTP_RESPONSE_H

#include <unordered_map>
#include <string>

namespace http {
    class http_response {
    public:

    private:
        std::unordered_map<std::string, std::string> headers;
        int status;
        std::string body;
        void prepare_headers();


    };
}

#endif //TEST_HTTP_RESPONSE_H
