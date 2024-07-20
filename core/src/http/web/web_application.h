//
// Created by chopk on 06.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_WEB_APPLICATION_H
#define SIMPLE_HTTP_SERVER_WEB_APPLICATION_H

#include <functional>
#include <vector>
#include "router/uri_dispatcher.h"
#include "network/tcp_server.h"
#include <boost/asio.hpp>
#include <memory>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

namespace http::web {
    class web_application : std::enable_shared_from_this<web_application> {
    public:
        explicit web_application(boost::asio::io_context& io_context, const int port = 8080)
        : port_(port) {
            router_ = std::make_shared<uri_dispatcher>();
            request_handler_ = std::make_shared<http::http_request_handler>(router_);
            server_ = std::make_unique<network::tcp_server>(io_context, port, http_protocol_factory(request_handler_));
        }

        void add_route(const std::string& path, const handler_t & handler, http_methods http_method);
        awaitable<void> handle_request(http::Request &request);
        void add_middleware(const std::function<void(raw_request_message)>& middleware_handler);
        awaitable<void> start(int port = 8080);
    private:
       std::shared_ptr<uri_dispatcher> router_ = nullptr;
       std::vector<std::function<void(raw_request_message)>> middlewares_;
       std::unique_ptr<network::tcp_server> server_ = nullptr;
       std::shared_ptr<http::http_request_handler> request_handler_;
       int port_;
    };
}




#endif //SIMPLE_HTTP_SERVER_WEB_APPLICATION_H
