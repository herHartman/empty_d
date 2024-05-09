//
// Created by chopk on 11.04.24.
//

#ifndef TEST_HTTP_REQUEST_HANDLER_H
#define TEST_HTTP_REQUEST_HANDLER_H

#include "http/web/router/uri_dispatcher.h"
#include "definitions/boost_asio_types.h"
#include "http/protocol/request/http_request.h"
#include <memory>
#include <boost/asio.hpp>


namespace http {

    class http_request_handler {
    public:
        explicit http_request_handler(
            const std::shared_ptr<http::web::uri_dispatcher>& uri_dispatcher
        ) : uri_dispatcher_(uri_dispatcher) {}

        awaitable<http_response> handle_request(request& request);
    private:
        std::shared_ptr<http::web::uri_dispatcher> uri_dispatcher_;
    };

} // http

#endif //TEST_HTTP_REQUEST_HANDLER_H
