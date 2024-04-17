//
// Created by chopk on 10.04.24.
//

#ifndef TEST_HTTP_RESPONSE_H
#define TEST_HTTP_RESPONSE_H


#include "../../web/http_headers.h"
#include "../../web/http_status.h"
#include "../../../io/serializable/json_serializable.h"
#include <unordered_map>
#include <string>
#include <type_traits>

namespace http {
    class http_response {
    public:

        explicit http_response(
            http::web::http_headers headers,
            http::web::http_status status,
            std::string body, std::size_t content_length
        ) : headers_(std::move(headers)), status_(status), body_(std::move(body)), content_length_(content_length) {}

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
    concept is_serializable = std::is_base_of_v<json_serializable<T, typename T::serializer>, T>;

    template<is_serializable T>
    http_response make_json_response(T object, http::web::http_status status) {
        std::string response_body = typename T::serializer{}.serialize(object);
        return http_response{http::web::http_headers{}, status, response_body, response_body.size()};
    }

    template<is_serializable T>
    http_response make_json_response(T object, http::web::http_status status, const http::web::http_headers& headers) {
        std::string response_body = typename T::serializer{}.serialize(object);
        return http_response{headers, status, response_body, response_body.size()};
    }

    class test_serializer;

    struct Test : public json_serializable<Test, test_serializer> {

    };

    class test_serializer {
    public:
        [[nodiscard]] std::string serialize(Test object) const {
            return {};
        }

        void test() {
            auto response = make_json_response(Test{}, http::web::http_status::HTTP_ACCEPTED);
        }
    };
}

#endif //TEST_HTTP_RESPONSE_H
