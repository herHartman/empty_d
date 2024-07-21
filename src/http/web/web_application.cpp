//
// Created by chopk on 06.04.24.
//

#include "web_application.h"

void http::web::web_application::add_route(
    const std::string& path,
    const handler_t & handler,
    HttpMethods http_method
) {
    router_->add_router(path, handler, http_method);
}

awaitable<void> http::web::web_application::handle_request(http::Request &request) {
    std::optional<resource_route> route = router_->resolve(request);
    if (route) {
        handler_t handler = route.value().get_handler();
        co_await handler(request);
    }
    co_return;
}

void http::web::web_application::add_middleware(const std::function<void(raw_request_message)> &middleware_handler) {

}

awaitable<void> http::web::web_application::start(const int port) {
    co_await server_->start();
}

