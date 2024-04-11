//
// Created by chopk on 11.04.24.
//

#ifndef TEST_HTTP_HEADERS_H
#define TEST_HTTP_HEADERS_H

#include <unordered_map>
#include <string>

namespace http::web {
    class http_headers {
    public:
        class headers {
            inline static constexpr char accept[] = "Accept";
            inline static constexpr char accept_charset[] = "Accept-Charset";
            inline static char accept_encoding[] = "Accept-Encoding";
            inline static char accept_language[] = "Accept-Language";
            inline static char accept_ranges[] = "Accept-Ranges";
            inline static char age[] = "Age";
            inline static char allow[] = "Allow";
            inline static char alternates[] = "Alternates";
            inline static char authorization[] = "Authorization";
            inline static char cache_control[] = "Cache-Control";
            inline static char connection[] = "Connection";
        };

    void set_basic_auth(const std::string& username, const std::string& password);
    void set_bearer_auth(const std::string& token);
    void set_connection(const std::string& connection);
    void set_content_length(std::size_t content_size);

    // TODO (add media_type class)
    void set_content_type(const std::string& media_type);
    // TODO (add date wrapper class)
    void set_date(const std::string& date);
    // TODO (add host wrapper class)
    void set_host(const std::string& host);
    // TODO (add uri class)
    void set_location(const std::string& location);
    void set_origin(const std::string& origin);

    void add(const std::string& header_name, const std::string& header_value);

    private:
        std::unordered_map<std::string, std::string> headers_;
    };
} // http

#endif //TEST_HTTP_HEADERS_H
