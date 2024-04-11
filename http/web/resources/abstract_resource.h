//
// Created by chopk on 05.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_ABSTRACT_RESOURCE_H
#define SIMPLE_HTTP_SERVER_ABSTRACT_RESOURCE_H

#include <string>
#include "../../protocol/request/raw_request_message.h"
#include "../router/resource_route.h"
#include "../../handlers/handlers.h"
#include <optional>
#include <functional>

namespace http::web {
    class abstract_resource {
    public:
        virtual std::optional<resource_route> resolve(const raw_request_message& request_message) = 0;
        virtual bool match(const std::string& path) = 0;
        virtual void add_route(const std::string& path, const handler_t& handler, http_methods http_method) = 0;
        virtual ~abstract_resource() = default;
    };
}




#endif //SIMPLE_HTTP_SERVER_ABSTRACT_RESOURCE_H
