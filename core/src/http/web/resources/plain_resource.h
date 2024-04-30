//
// Created by chopk on 05.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_PLAIN_RESOURCE_H
#define SIMPLE_HTTP_SERVER_PLAIN_RESOURCE_H


#include "abstract_resource.h"
#include "http/web/router/resource_route.h"
#include "http/handlers/handlers.h"
#include <utility>
#include <vector>

namespace http::web {
    class plain_resource : public abstract_resource {
    public:
        explicit plain_resource(std::string  path) : path_(std::move(path)) {}
        bool match(const std::string& path) override;
        std::optional<resource_route> resolve(http_request& request_message) override;
        void add_route(const std::string& path, const handler_t& handler, http_methods http_method) override;
    private:
        std::string path_;
        std::vector<resource_route> routes_{};
    };
}


#endif //SIMPLE_HTTP_SERVER_PLAIN_RESOURCE_H
