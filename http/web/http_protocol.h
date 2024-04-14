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
    : transport_(std::move(transport)), request_handler_(request_handler) {
        http_writer_ = std::make_unique<http::http_writer>(transport_);
    }

    boost::asio::awaitable<void> handle() {
        std::cout << "spawn coroutine\n";
        try {
            while (transport_->is_open()) {
                char data[1024];
                memset(data, 0, 1024);
                auto buffer = boost::asio::buffer(data);
                std::size_t data_len = co_await transport_->read(buffer);
                std::size_t last_message_index = request_parser_.parse_message(data, data_len, raw_request_message_);
                if (request_parser_.is_parse_message_complete()) {
                    co_spawn(transport_.)
                    http::http_response response = co_await request_handler_->handle_request(raw_request_message_);
                    std::string status_line = "HTTP/1.1 200 HTTP_OK\r\n";
                    std::cout << "test\n";
                    co_await transport_->write(boost::asio::buffer(status_line + response.format_headers()));
                    transport_->close();
                } else if (request_parser_.is_parse_message_complete())
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
    std::unique_ptr<http::http_writer> http_writer_ = nullptr;

    awaitable<http::http_response> handle_request(const http::raw_request_message& message) {
        auto response = co_await request_handler_->handle_request(raw_request_message_);
        co_return response;
    }

};


#endif //SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
