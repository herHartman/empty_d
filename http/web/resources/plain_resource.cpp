//
// Created by chopk on 05.04.24.
//

#include "plain_resource.h"
#include "../../handlers/handlers.h"

namespace http::web {
    bool plain_resource::match(const std::string &path) {
        return path == path_;
    }

    std::optional<resource_route> plain_resource::resolve(const http::raw_request_message &request_message) {
        if (!match(request_message.path)) {
            return std::nullopt;
        }

        for (auto & route : routes_) {
            if (route.get_method() == request_message.method) {
                return route;
            }
        }

        return std::nullopt;
    }

    void plain_resource::add_route(
        const std::string &path,
        const handler_t& handler,
        http::http_methods http_method
    ) {
        if (!match(path)) {
            throw std::exception();
        }

        for (const auto & route : routes_) {
            if (route.get_method() == http_method) {
                throw std::exception();
            }
        }

        routes_.emplace_back(path, handler, http_method);
    }

}

