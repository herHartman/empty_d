//
// Created by chopk on 06.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_HANDLERS_H
#define SIMPLE_HTTP_SERVER_HANDLERS_H

#include "../protocol/response/http_response.h"
#include "../protocol/request/http_request.h"
#include <functional>
#include <boost/asio.hpp>

using boost::asio::awaitable;

typedef std::function<awaitable<http::http_response>(http::Request&)> handler_t;

http::http_response handle_request(http::Request& request_message);

#endif //SIMPLE_HTTP_SERVER_HANDLERS_H
