//
// Created by chopk on 06.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
#define SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H

#include "network/transport.h"
#include "http/handlers/http_request_handler.h"
#include "http/protocol/request/http_request.h"
#include "http/protocol/request/http_request_parser.h"
#include "http/protocol/http_writer.h"
#include <array>
#include <boost/asio.hpp>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;
typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;

class base_protocol {
public:
  explicit base_protocol(
      network::transport::transport_p transport,
      const std::shared_ptr<http::http_request_handler> &request_handler)
      : transport_(std::move(transport)), request_handler_(request_handler),
        request_parser_(), buffer_{} {
    http_writer_ = std::make_unique<http::http_writer>(transport_);
  }

  boost::asio::awaitable<void> handle(); 
  void connection_made() {}

  void connection_lost() {}

private:
  network::transport::transport_p transport_;
  http::http_request_parser request_parser_;
  http::raw_request_message raw_request_message_;
  std::shared_ptr<http::http_request_handler> request_handler_;
  std::unique_ptr<http::http_writer> http_writer_ = nullptr;
  std::array<char, 8192> buffer_;

  awaitable<void> handle_request(http::request &request) {
    auto response = co_await request_handler_->handle_request(request);
    std::string status_line = "HTTP/1.1 200 HTTP_OK\r\n";

    co_await transport_->write(
        boost::asio::buffer(status_line + response.format_headers()));

    auto stream_reader = request.get_stream_reader();
    while (!stream_reader->is_eof()) {
      co_await stream_reader->read_any();
    }

    transport_->close();
    co_return;
  }
};

#endif // SIMPLE_HTTP_SERVER_HTTP_PROTOCOL_H
