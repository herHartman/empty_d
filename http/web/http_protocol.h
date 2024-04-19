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
        const std::shared_ptr<http::http_request_handler>& request_handler
    )
    :   transport_(std::move(transport)), request_handler_(request_handler),
        request_parser_()  {
        http_writer_ = std::make_unique<http::http_writer>(transport_);
    }

    boost::asio::awaitable<void> handle() {
        try {
            while (transport_->is_open()) {
                std::size_t data_len = co_await transport_->read(boost::asio::buffer(buffer_));
                if (!request_parser_.is_parse_message_complete()) {
                    std::size_t last_message_index = request_parser_.parse_message(
                        buffer_.data(), data_len, raw_request_message_
                    );
                    if (request_parser_.is_parse_message_complete()) {

                        auto stream_reader = std::make_shared<http::http_body_stream_reader>(
                            std::make_shared<http::http_body_stream_reader::read_lock_channel>(
                                transport_->get_executor(), 1
                            )
                        );

                        http::http_request request(stream_reader);
                        request_parser_.set_payload(stream_reader);
                        co_spawn(transport_->get_executor(), handle_request(raw_request_message_, stream_reader), detached);
                        if (last_message_index < data_len) {
                            co_await request_parser_.parse_body(&buffer_[last_message_index + 1], data_len - last_message_index - 1, raw_request_message_);
                        }
                    }
                } else {
                    co_await request_parser_.parse_body(buffer_.data(), data_len, raw_request_message_);
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

    std::array<char, 8192> buffer_;

    awaitable<void> handle_request(
        const http::raw_request_message& message,
        std::shared_ptr<http::http_body_stream_reader> stream_reader
    ) {
        auto response = co_await request_handler_->handle_request(message);
        std::string status_line = "HTTP/1.1 200 HTTP_OK\r\n";

        co_await transport_->write(boost::asio::buffer(status_line + response.format_headers()));

        while (!stream_reader->is_eof()) {
            co_await stream_reader->read_any();
        }

         transport_->close();
         co_return;
    }

};


#endif //SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
