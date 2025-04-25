#pragma once

#include "http/http_response.hpp"
#include "http/protocol/parser/http_request_parser.hpp"
#include "http/protocol/request/http_request.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <utility>


namespace empty_d::http {
using boost::asio::ip::tcp;
typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;

using FixedBodyHttpHandler = std::function<HttpResponse(
  empty_d::http::request::HttpRequest &, boost::asio::yield_context yield)>;

class HttpStreamResponse {};

class BaseHttpRequestHandler {
public:
  virtual HttpResponseBase handleRequest(HttpRequest &request,
                                     boost::asio::yield_context yield) = 0;
};

class BaseHttpStreamRequestHandler {
public:
  virtual void handleRequest(HttpRequest &request, HttpStreamResponse &body,
                             boost::asio::yield_context yield) = 0;
};

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
  explicit HttpConnection(protocol::parser::HttpRequestParser request_parser,
                          tcp::socket socket)
      : request_parser_(std::move(request_parser)), socket_(std::move(socket)),
        write_buffer_{} {}

  void readRequestBody(boost::asio::yield_context yield,
                       std::shared_ptr<request::HttpBodyStreamReader> body);
  void handle(boost::asio::yield_context yield);

  void ConnectionMade() {}

  void ConnectionLost() {}

private:
  void processRequest(FixedBodyHttpHandler handler, HttpRequest &request,
                      boost::asio::yield_context yield);
  
  void processRequest(StreamBodyHttpHandler handler, HttpRequest &request,
                      boost::asio::yield_context yield);

  tcp::socket socket_;
  protocol::parser::HttpRequestParser request_parser_;
  std::string bucket_{};
  boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                     std::allocator<char>>
      read_buffer_{bucket_};

  void HandleRequest(HttpRequest &request);
  std::string write_buffer_;
};
} // namespace empty_d::http

