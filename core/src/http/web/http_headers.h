//
// Created by chopk on 11.04.24.
//

#ifndef TEST_HTTP_HEADERS_H
#define TEST_HTTP_HEADERS_H

#include <initializer_list>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>

namespace http::web {
    typedef std::variant<std::string_view, const std::string> header_value; 
    typedef std::variant<std::string_view, const std::string> header_key;
    typedef std::unordered_map<header_key, std::vector<header_value>> header_values; 

    class headers {
    public:
        class basic_headers {
            inline static constexpr char accept[] = "Accept";
            inline static constexpr char accept_charset[] = "Accept-Charset";
            inline static char accept_encoding[] = "Accept-Encoding";
            inline static char accept_language[] = "Accept-Language";
            inline static char accept_ranges[] = "Accept-Ranges";
            inline static char age[] = "Age";
            inline static char allow[] = "Allow";
            inline static const std::string_view alternates = "Alternates";
            inline static const std::string_view authorization = "Authorization";
            inline static const std::string_view cache_control = "Cache-Control";
            inline static const std::string_view connection = "Connection";
        };
    
    void set(header_key key, header_value value);
    void set(header_key key, std::vector<header_value> values);
    
    void add(header_key key, header_value value);
    void add(header_key key, std::vector<header_value> values);

    void set_basic_auth(const std::string& username, const std::string& password);
    void set_bearer_auth(const std::string& token);
    void set_connection(const std::string& connection);
    void set_content_length(std::size_t content_size); // TODO (add media_type class)
    void set_content_type(const std::string& media_type);
    // TODO (add date wrapper class)
    void set_date(const std::string& date);
    // TODO (add host wrapper class)
    void set_host(const std::string& host);
    // TODO (add uri class)
    void set_location(const std::string& location);
    void set_origin(const std::string& origin);


    std::string format_headers() const;

    private:
       header_values headers_;
    };
} // http

#endif //TEST_HTTP_HEADERS_H
