//
// Created by chopk on 11.04.24.
//

#include "http_request_handler.h"

namespace http {
    awaitable<http_response> http_request_handler::handle_request(const raw_request_message &request_message) {
        std::optional<web::resource_route> route = uri_dispatcher_->resolve(request_message);
        if (route) {
            handler_t handler = route.value().get_handler();
            http_response test = co_await handler(request_message);
            co_return test;
        } else {
            throw std::exception();
        }
    }
} // http