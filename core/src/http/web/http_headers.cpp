//
// Created by chopk on 11.04.24.
//

#include "http_headers.h"
#include <sstream>
#include <vector>

namespace http::web {
    
    void headers::set(header_key key, header_value value) {
        headers_.erase(key);
        std::vector<header_value> values{std::move(value)};
        values.reserve(1);
        headers_.insert({std::move(key), values});
    }
    
    void headers::set(header_key key, std::vector<header_value> values) {
        headers_.erase(key);
        headers_.insert({std::move(key), std::move(values)});
    }
    
    void headers::add(header_key key, header_value value) {
       auto entry = headers_.find(key);
       if (entry == headers_.end()) {
            set(std::move(key), std::move(value));
       } else {
            entry->second.push_back(std::move(value)); 
       }
    }

    void headers::add(header_key key, std::vector<header_value> values) {
        auto entry = headers_.find(key);
        if (entry == headers_.end()) {
            set(std::move(key), std::move(values));
        } else {
            for (const auto& iter : values) {
                entry->second.push_back(iter);
            }
        }
    }

    std::string headers::format_headers() const {
        std::stringstream buf;
        for (const auto & header : headers_) {
        }
        buf << "\r\n";
        return buf.str();
    }
}
