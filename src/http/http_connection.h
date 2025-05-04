#pragma once

#include "http/http_response.hpp"
#include "http/parser/http_request_parser.hpp"
#include "http/url_dispatcher.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>

namespace empty_d::http {

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
  HttpConnection(std::shared_ptr<UrlDispatcher> urlDispatcher,
                 tcp::socket socket);

  void readRequestBody(boost::asio::yield_context yield,
                       protocol::parser::HttpRequestParser &requestParser);
  void handle(boost::asio::yield_context yield);

  void ConnectionMade() {}

  void ConnectionLost() {}

  void processRequest(std::unique_ptr<HttpHandlerBase> handler,
                      HttpRequest &request, boost::asio::yield_context yield);
  void processRequest(std::unique_ptr<StreamResponseHttpHandlerBase> handler,
                      HttpRequest &request, boost::asio::yield_context yield);

private:
  void readAndParseData(protocol::parser::HttpRequestParser &requestParser,
                        size_t bufferSize, boost::asio::yield_context yield);

  std::shared_ptr<UrlDispatcher> mUrlDispatcher;
  tcp::socket mSocket;
  std::string mBucket{};
  boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                     std::allocator<char>>
      mReadBuffer{mBucket};
};
} // namespace empty_d::http
