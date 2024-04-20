//
// Created by chopk on 05.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_URI_DISPATCHER_H
#define SIMPLE_HTTP_SERVER_URI_DISPATCHER_H

#include "../../protocol/request/raw_request_message.h"
#include "../resources/abstract_resource.h"
#include "resource_route.h"
#include "../../protocol/response/http_response.h"
#include "../../handlers/handlers.h"
#include <vector>
#include <memory>

namespace http::web {

    class uri_dispatcher {
    public:
        std::optional<resource_route> resolve(http_request& request);
        void add_router(
            const std::string& path,
            const handler_t& handler,
            http_methods method
        );
    private:
        std::vector<std::shared_ptr<abstract_resource>> resources_;
    };

} // http

#endif //SIMPLE_HTTP_SERVER_URI_DISPATCHER_H
