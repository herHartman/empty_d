//
// Created by chopk on 11.04.24.
//

#include "http_headers.h"
#include <sstream>

namespace http::web {

    void http_headers::set_basic_auth(const std::string &username, const std::string &password) {

    }

    void http_headers::set_bearer_auth(const std::string &token) {

    }

    void http_headers::add(const std::string &header_name, const std::string &header_value) {

    }

    void http_headers::set_origin(const std::string &origin) {

    }

    void http_headers::set_location(const std::string &location) {

    }

    void http_headers::set_host(const std::string &host) {

    }

    void http_headers::set_date(const std::string &date) {

    }

    void http_headers::set_content_type(const std::string &media_type) {

    }

    void http_headers::set_content_length(std::size_t content_size) {

    }

    void http_headers::set_connection(const std::string &connection) {

    }

    std::string http_headers::format_headers() const {
        std::stringstream buf;
        for (const auto & header : headers_) {
            buf << header.first << ": " << header.second << "\r\n";
        }
        return buf.str();
    }
}