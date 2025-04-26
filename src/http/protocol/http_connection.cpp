#include "http/protocol/http_connection.h"
#include "http/http_response.hpp"
#include "http/url_dispatcher.hpp"
#include <algorithm>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/impl/spawn.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/functional/overloaded_function.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/variant.hpp>
#include <memory>

namespace empty_d::http {

using FixedBodyHttpHandler = std::function<HttpResponse(
    empty_d::http::request::HttpRequest &, boost::asio::yield_context yield)>;

void HttpConnection::readRequestBody(
    boost::asio::yield_context yield,
    std::shared_ptr<request::HttpBodyStreamReader> body) {

  boost::asio::socket_base::receive_buffer_size option;
  socket_.get_option(option);
  size_t osBufferSize = option.value();
  
  while (!body->isEof()) {
    size_t availableSpace = read_buffer_.max_size() - read_buffer_.size();
    size_t readSize = std::min(availableSpace, osBufferSize);

    // Читаем данные (async_read_some)
    boost::asio::mutable_buffer buffer = read_buffer_.prepare(readSize);
    size_t bytesTransferred =
        socket_.async_read_some(boost::asio::buffer(buffer), yield);

    // Фиксируем прочитанные данные
    read_buffer_.commit(bytesTransferred);
    body->write(bucket_, yield);
    read_buffer_.consume(readSize);
  }
}

void HttpConnection::handle(boost::asio::yield_context yield) {
  boost::asio::socket_base::receive_buffer_size option;
  socket_.get_option(option);
  size_t osBufferSize = option.value();

  try {
    while (!request_parser_.ParseComplete()) {

      // Рассчитываем размер чтения
      size_t availableSpace = read_buffer_.max_size() - read_buffer_.size();
      size_t readSize = std::min(availableSpace, osBufferSize);

      // Читаем данные (async_read_some)
      boost::asio::mutable_buffer buffer = read_buffer_.prepare(readSize);
      size_t bytesTransferred =
          socket_.async_read_some(boost::asio::buffer(buffer), yield);

      // Фиксируем прочитанные данные
      read_buffer_.commit(bytesTransferred);
      request_parser_.Parse(bucket_.data(), bytesTransferred);
      read_buffer_.consume(readSize);
    }
  } catch (const std::exception &e) {
    std::printf("echo Exception: %s\n", e.what());
  }

  std::pair<HttpRequest, empty_d::http::HttpHandler> buidRequestResult =
      request_parser_.BuildRequest();

  std::shared_ptr<request::HttpBodyStreamReader> body_reader =
      buidRequestResult.first.GetStreamReader();

  boost::asio::spawn(
      socket_.get_executor(),
      [this, body_reader](boost::asio::yield_context yield) mutable {
        readRequestBody(yield, body_reader);
      });

  socket_.close();
}

void HttpConnection::processRequest(std::unique_ptr<HttpHandlerBase> handler,
                                    HttpRequest &request,
                                    boost::asio::yield_context yield) {
  HttpResponse response = handler->handleRequest(request, yield);
  boost::asio::async_write(
      socket_, boost::asio::buffer(response.serializeHeaders()), yield);
  boost::asio::async_write(
      socket_, boost::asio::buffer(response.serializeResponse()), yield);
}

void HttpConnection::processRequest(
    std::unique_ptr<StreamResponseHttpHandlerBase> handler,
    HttpRequest &request, boost::asio::yield_context yield) {
  StreamHttpResponse response;
  boost::asio::spawn(socket_.get_executor(),
                     [this, handler = std::move(handler), &request,
                      &response](boost::asio::yield_context yield) mutable {
                       handler->handleRequest(request, response, yield);
                     });

  response.awaitEndOfHeaders(yield);
  boost::asio::async_write(
      socket_, boost::asio::buffer(response.serializeHeaders()), yield);

  while (not response.isEndOfStream(yield)) {
    std::string nextChunk = response.readChunk(yield);
    boost::asio::async_write(socket_, boost::asio::buffer(nextChunk), yield);
  }
}

} // namespace empty_d::http