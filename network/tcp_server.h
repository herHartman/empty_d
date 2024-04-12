//
// Created by chopk on 19.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_TCP_SERVER_H
#define SIMPLE_HTTP_SERVER_TCP_SERVER_H

#include "../http/protocol/request/http_request_parser.h"
#include "transport.h"
#include "../http/web/http_protocol.h"
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;
using boost::asio::detached;


class http_protocol_factory {
public:
    explicit http_protocol_factory(const std::shared_ptr<http::http_request_handler>& request_handler)
        : request_handler_(request_handler) {}

    base_protocol create_protocol(network::transport::transport_p transport) {
        return base_protocol(std::move(transport), request_handler_);
    }
private:
    std::shared_ptr<http::http_request_handler> request_handler_;
};

namespace network {
    class tcp_server {
    public:
        explicit tcp_server(boost::asio::io_context& io_context, const int port, http_protocol_factory factory)
            :   acceptor_(io_context, tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
                protocol_factory_(std::move(factory)) {}

        boost::asio::awaitable<void> start() {
            for (;;) {
                tcp::socket client_socket = co_await acceptor_.async_accept(boost::asio::use_awaitable);
                auto protocol = protocol_factory_.create_protocol(std::make_shared<transport>(std::move(client_socket)));
                co_await protocol.handle();
            }
        }

    private:
        tcp::acceptor acceptor_;
        http_protocol_factory protocol_factory_;
    };
}



#endif // SIMPLE_HTTP_SERVER_TCP_SERVER_H
