//
// Created by chopk on 10.04.24.
//

#ifndef TEST_HTTP_RESPONSE_H
#define TEST_HTTP_RESPONSE_H


#include "../../web/http_headers.h"
#include "../../web/http_status.h"
#include <unordered_map>
#include <string>

namespace http {
    class http_response {
    public:
        std::string format_headers() const { return headers_.format_headers(); }
        void prepare_headers();

        void set_cookie(const std::string& name, const std::string& value);
        std::string get_cookie() const;

        http::web::http_status get_status() const { return status_; }
        void set_status(http::web::http_status status) { status_ = status; }

    private:
        http::web::http_headers headers_;
        http::web::http_status status_;
        std::string body_;
        std::size_t content_length_;
    };

    template<typename T>
    http_response make_json_response(T object) {

    }

}

#endif //TEST_HTTP_RESPONSE_H
