#pragma once

#include "http/protocol/parser/http_request_parser.hpp"
#include "http/transport.h"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <memory>
#include <utility>

namespace empty_d::http {
using boost::asio::ip::tcp;
typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;
using boost::asio::experimental::channel;

class HttpConnection {
public:
  explicit HttpConnection(protocol::parser::HttpRequestParser request_parser,
                          tcp::socket socket)
      : request_parser_(std::move(request_parser)), socket_(std::move(socket)) {
  }

  awaitable<void> Handle();

  void ConnectionMade() {}

  void ConnectionLost() {}

private:
  tcp::socket socket_;
  protocol::parser::HttpRequestParser request_parser_;
  std::string bucket_{};
  boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                     std::allocator<char>>
      read_buffer_{bucket_};
  channel<void(boost::system::error_code, std::size_t)> response_awaiter_{socket_.get_executor()};
  awaitable<void> HandleRequest(HttpRequest &request);
};
} // namespace empty_d::http
