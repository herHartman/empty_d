//
// Created by chopk on 11.04.24.
//

#include "http_request_handler.h"
#include "../protocol/request/http_request.h"

namespace http {
    awaitable<http_response> http_request_handler::handle_request(http_request& request) {
        std::optional<web::resource_route> route = uri_dispatcher_->resolve(request);
        if (route) {
            handler_t handler = route.value().get_handler();
            http_response test = co_await handler(request);
            co_return test;
        } else {
            throw std::exception();
        }
    }
} // http