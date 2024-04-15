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
#include "../protocol/request/http_request.h"

using boost::asio::ip::tcp;
typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;


class base_protocol {
public:

    explicit base_protocol(
        network::transport::transport_p transport,
        const std::shared_ptr<http::http_request_handler>& request_handler,
        http::http_body_stream_reader::read_lock_channel_p read_lock_channel
    )
    :   transport_(std::move(transport)), request_handler_(request_handler),
        request_parser_(std::move(read_lock_channel))  {
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
                if (!request_parser_.is_parse_message_complete()) {
                    std::size_t last_message_index = request_parser_.parse_message(data, data_len, raw_request_message_);
                    if (request_parser_.is_parse_message_complete()) {
                        if (raw_request_message_.has_body()) {
                            stream_buf_ = std::make_shared<boost::asio::streambuf>();
                        }

                        http::http_body_stream_reader stream_reader(
                            request_parser_.get_read_lock_channel(),
                            stream_buf_
                        );

                        http::http_request request(stream_reader);

                        co_spawn(transport_->get_executor(), handle_request(raw_request_message_), detached);
                        if (last_message_index < data_len) {
                            request_parser_.parse_body(&data[last_message_index], data_len, raw_request_message_);
                        }

                    }
                } else {
                    request_parser_.parse_body(data, data_len, raw_request_message_);
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

    std::unique_ptr<http::http_writer> http_writer_ = nullptr;
    std::shared_ptr<boost::asio::streambuf> stream_buf_ = nullptr;

    awaitable<void> handle_request(const http::raw_request_message& message) {
        auto response = co_await request_handler_->handle_request(message);
        std::string status_line = "HTTP/1.1 200 HTTP_OK\r\n";
        co_await transport_->write(boost::asio::buffer(status_line + response.format_headers()));
        transport_->close();
        co_return;
    }

};


#endif //SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
