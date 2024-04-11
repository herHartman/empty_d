//
// Created by chopk on 06.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_HANDLERS_H
#define SIMPLE_HTTP_SERVER_HANDLERS_H

#include "../protocol/request/raw_request_message.h"
#include "../protocol/response/http_response.h"
#include <functional>
#include <boost/asio.hpp>

using boost::asio::awaitable;

typedef std::function<awaitable<http::http_response>(const http::raw_request_message&)> handler_t;

http::http_response handle_request(const http::raw_request_message& request_message);

#endif //SIMPLE_HTTP_SERVER_HANDLERS_H
