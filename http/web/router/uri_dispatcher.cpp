//
// Created by chopk on 05.04.24.
//

#include "uri_dispatcher.h"
#include "../resources/plain_resource.h"
#include <memory>
#include <optional>


namespace http::web {

    std::optional<resource_route> uri_dispatcher::resolve(const raw_request_message& request) {
        for (auto & resource : resources_) {
            std::optional<resource_route> route = resource->resolve(request);
            if (route) return route;
        }
        return std::nullopt;
    }

    void uri_dispatcher::add_router(
        const std::string& path,
        const handler_t& handler,
        http_methods method
    ) {
        for (auto & resource : resources_) {
            if (resource->match(path)) {
                resource->add_route(path, handler, method);
                return;
            }
        }
        auto resource = std::make_shared<plain_resource>(path);\
        resource->add_route(path, handler, method);
        resources_.push_back(resource);
    }
} // http