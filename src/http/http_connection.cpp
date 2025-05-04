#include "http/http_connection.h"
#include "http/http_response.hpp"
#include "http/parser/http_request_parser.hpp"
#include "http/url_dispatcher.hpp"
#include <algorithm>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/impl/spawn.hpp>
#include <boost/asio/read_at.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/functional/overloaded_function.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/variant.hpp>
#include <memory>

namespace empty_d::http {

HttpConnection::HttpConnection(std::shared_ptr<UrlDispatcher> urlDispatcher,
                               tcp::socket socket)
    : mUrlDispatcher(std::move(urlDispatcher)), mSocket(std::move(socket)) {}

void HttpConnection::readAndParseData(
    protocol::parser::HttpRequestParser &requestParser, size_t bufferSize,
    boost::asio::yield_context yield) {
  // Рассчитываем размер чтения
  size_t availableSpace = mReadBuffer.max_size() - mReadBuffer.size();
  size_t readSize = std::min(availableSpace, bufferSize);

  // Читаем данные (async_read_some)
  boost::asio::mutable_buffer buffer = mReadBuffer.prepare(readSize);
  size_t bytesTransferred =
      boost::asio::async_read(mSocket, boost::asio::buffer(buffer), yield);

  // Фиксируем прочитанные данные
  mReadBuffer.commit(bytesTransferred);
  requestParser.parse(mBucket.data(), bytesTransferred);
  mReadBuffer.consume(readSize);
}

void HttpConnection::readRequestBody(
    boost::asio::yield_context yield,
    protocol::parser::HttpRequestParser &parser) {

  boost::asio::socket_base::receive_buffer_size option;
  mSocket.get_option(option);
  size_t osBufferSize = option.value();

  while (not parser.parseComplete()) {
    readAndParseData(parser, osBufferSize, yield);
  }
}

void HttpConnection::handle(boost::asio::yield_context yield) {
  boost::asio::socket_base::receive_buffer_size option;
  mSocket.get_option(option);
  size_t osBufferSize = option.value();

  http::protocol::parser::HttpRequestParser requestParser{
      mUrlDispatcher, mSocket.get_executor()};

  boost::asio::steady_timer readTimer(mSocket.get_executor());
  boost::system::error_code ec;

  while (not requestParser.parseComplete()) {
    readAndParseData(requestParser, osBufferSize, yield);
  }

  std::pair<HttpRequest, empty_d::http::HttpHandler> buidRequestResult =
      requestParser.buildRequest();

  boost::asio::spawn(
      mSocket.get_executor(),
      [this, &requestParser](boost::asio::yield_context yield) mutable {
        readRequestBody(yield, requestParser);
      });

  buidRequestResult.second(shared_from_this(), buidRequestResult.first, yield);

  mSocket.close();
}

void HttpConnection::processRequest(std::unique_ptr<HttpHandlerBase> handler,
                                    HttpRequest &request,
                                    boost::asio::yield_context yield) {
  HttpResponse response = handler->handleRequest(request, yield);
  response.sendResponse(mSocket, yield);
}

} // namespace empty_d::http