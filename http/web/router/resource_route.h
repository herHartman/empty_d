//
// Created by chopk on 05.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_RESOURCE_ROUTE_H
#define SIMPLE_HTTP_SERVER_RESOURCE_ROUTE_H

#include "../../protocol/http_methods.h"
#include "../../protocol/request/raw_request_message.h"
#include "../../handlers/handlers.h"
#include <string>
#include <functional>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::awaitable;

namespace http::web {
    class resource_route {
    public:
        explicit resource_route(
            std::string path,
            handler_t handler,
            http_methods http_method
        ) : http_method_(http_method), path_(std::move(path)), handler_(std::move(handler)) {}

        [[nodiscard]] http_methods get_method() const { return http_method_; }
        [[nodiscard]] std::string get_path() const { return path_; }
        [[nodiscard]] handler_t get_handler() const { return handler_; }

    private:
        const http_methods http_method_;
        std::string path_;
        handler_t handler_;
    };
}


#endif //SIMPLE_HTTP_SERVER_RESOURCE_ROUTE_H
