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
#include <stdexcept>

namespace empty_d::http {

constexpr size_t kMaxMessageSize = 16 * 1024;

HttpConnection::HttpConnection(std::shared_ptr<UrlDispatcher> urlDispatcher,
                               tcp::socket socket)
    : mUrlDispatcher(std::move(urlDispatcher)), mSocket(std::move(socket)) {}

size_t HttpConnection::readAndParseData(
    protocol::parser::HttpRequestParser &requestParser, size_t bufferSize,
    boost::asio::yield_context yield) {
  // Рассчитываем размер чтения
  size_t availableSpace = mReadBuffer.max_size() - mReadBuffer.size();
  size_t readSize = std::min(availableSpace, bufferSize);

  // Читаем данные (async_read_some)
  boost::asio::mutable_buffer buffer = mReadBuffer.prepare(readSize);

  size_t bytesTransferred =
      mSocket.async_read_some(boost::asio::buffer(buffer), yield);
  // Фиксируем прочитанные данные
  mReadBuffer.commit(bytesTransferred);
  requestParser.parse(mBucket.data(), bytesTransferred);
  mReadBuffer.consume(readSize);

  return readSize;
}

void HttpConnection::readRequestBody(
    boost::asio::yield_context yield,
    protocol::parser::HttpRequestParser &parser) {

  boost::asio::socket_base::receive_buffer_size option;
  mSocket.get_option(option);
  size_t osBufferSize = option.value();

  std::size_t totalReadMessageSize{0};
  while (not parser.parseComplete()) {
    size_t readMessageSize = readAndParseData(parser, osBufferSize, yield);
    totalReadMessageSize += readMessageSize;
  }

  std::cout << "total receive bytes from body " << totalReadMessageSize
            << std::endl;
}

void HttpConnection::handle(boost::asio::yield_context yield) {
  boost::asio::socket_base::receive_buffer_size option;
  mSocket.get_option(option);
  size_t osBufferSize = option.value();

  http::protocol::parser::HttpRequestParser requestParser{
      mUrlDispatcher, mSocket.get_executor()};

  boost::asio::steady_timer readTimer(mSocket.get_executor());
  boost::system::error_code ec;

  std::size_t totalReadMessageSize{0};
  while (not(requestParser.parseMessageComplete() ||
             requestParser.parseComplete())) {
    readAndParseData(requestParser, osBufferSize, yield);
    if (totalReadMessageSize >= kMaxMessageSize) {
      throw std::runtime_error("receive large http message");
    }
  }

  std::cout << "total receive bytes from message " << totalReadMessageSize
            << std::endl;
  std::pair<HttpRequest, empty_d::http::HttpHandler> buidRequestResult =
      requestParser.buildRequest();

  if (not requestParser.parseComplete()) {
    boost::asio::spawn(
        mSocket.get_executor(),
        [this, &requestParser](boost::asio::yield_context yield) mutable {
          readRequestBody(yield, requestParser);
        });
  }

  buidRequestResult.second(shared_from_this(), buidRequestResult.first, yield);
}

void HttpConnection::processRequest(std::unique_ptr<HttpHandlerBase> handler,
                                    HttpRequest &request,
                                    boost::asio::yield_context yield) {
  HttpResponse response = handler->handleRequest(request, yield);
  response.sendResponse(mSocket, yield);
}

void HttpConnection::close() { mSocket.close(); }

const tcp::socket::executor_type &HttpConnection::getExecutor() {
  return mSocket.get_executor();
}

} // namespace empty_d::http