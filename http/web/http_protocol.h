//
// Created by chopk on 06.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
#define SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H

#include <boost/asio.hpp>
#include <memory>
#include <utility>
#include <iostream>
#include "../protocol/request/http_request_parser.h"
#include "../handlers/http_request_handler.h"
#include "../../network/transport.h"

using boost::asio::ip::tcp;
typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;


class base_protocol {
public:

    explicit base_protocol(
        network::transport::transport_p transport,
        const std::shared_ptr<http::http_request_handler>& request_handler
    )
    : transport_(std::move(transport)), request_handler_(request_handler) {}

    boost::asio::awaitable<void> handle() {
        try {
            for (;;) {
                char data[1024];
                memset(data, 0, 1024);
                auto buffer = boost::asio::buffer(data);
                std::size_t data_len = co_await socket_.async_read_some(buffer, boost::asio::use_awaitable);
                socket_.async_write_some()
                request_parser_.parse(data, data_len, raw_request_message_);
                if (request_parser_.is_complete()) {
                    co_await request_handler_->handle_request(raw_request_message_);
                }
            }
        } catch (std::exception& e) {
            std::printf("echo Exception: %s\n", e.what());
        }
    }

    void connection_made() {

    }

    void connection_lost() {

    }


private:
    network::transport::transport_p transport_;
    http::http_request_parser request_parser_;
    http::raw_request_message raw_request_message_;
    std::shared_ptr<http::http_request_handler> request_handler_;
    tcp::socket socket_;
};


#endif //SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
