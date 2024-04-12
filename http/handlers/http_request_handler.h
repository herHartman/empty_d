//
// Created by chopk on 11.04.24.
//

#ifndef TEST_HTTP_REQUEST_HANDLER_H
#define TEST_HTTP_REQUEST_HANDLER_H

#include "../web/router/uri_dispatcher.h"
#include "../../core/boost_asio_types.h"
#include "../protocol/http_writer.h"
#include <memory>
#include <boost/asio.hpp>
#include <utility>


namespace http {

    class http_request_handler {
    public:
        explicit http_request_handler(const std::shared_ptr<http::web::uri_dispatcher>& uri_dispatcher)
            : uri_dispatcher_(uri_dispatcher) {}

        awaitable<http_response> handle_request(const raw_request_message& request_message);
    private:
        std::shared_ptr<http::web::uri_dispatcher> uri_dispatcher_;
    };

} // http

#endif //TEST_HTTP_REQUEST_HANDLER_H
