//
// Created by chopk on 10.04.24.
//

#ifndef TEST_HTTP_RESPONSE_H
#define TEST_HTTP_RESPONSE_H

#include <unordered_map>
#include <string>
#include "../../web/http_headers.h"

namespace http {
    class http_response {
    public:
        std::string format_headers() const { return headers_.format_headers(); }
        void prepare_headers();
        void set_cookie(const std::string& name, const std::string& value);
        std::string get_cookie() const;
        
    private:
        http::web::http_headers headers_;
        int status;
        std::string body;
    };
}

#endif //TEST_HTTP_RESPONSE_H
