//
// Created by chopk on 06.04.24.
//

#include <iostream>
#include "handlers.h"

http::http_response handle_request(const http::raw_request_message& request_message) {
    std::cout << request_message.path;
}
