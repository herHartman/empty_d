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
#include <stdexcept>

namespace empty_d::http {

using FixedBodyHttpHandler = std::function<HttpResponse(
    empty_d::http::request::HttpRequest &, boost::asio::yield_context yield)>;

void HttpConnection::HandleRequest(HttpRequest &request) {
  // std::optional<Resource> resource = request_parser_.GetResource();
  // if (!resource) {
  //   throw std::runtime_error("resource is required");
  // }

  // HttpHandler handler = resource->GetHandler(request.GetMethod());
  // HttpResponse response = co_await handler(request);
  // write_buffer_.append(response.SerializeResponse());
  // response_awaiter_.try_send(0, 0);
}

void HttpConnection::readRequestBody(
    boost::asio::yield_context yield,
    std::shared_ptr<request::HttpBodyStreamReader> body) {
  while (!body->isEof()) {
    size_t limit = read_buffer_.max_size() - read_buffer_.size();
    size_t max_size = 65536;
    size_t read_size = std::min(std::max<size_t>(512, read_buffer_.capacity()),
                                std::min(limit, max_size));

    read_buffer_.prepare(read_size);
    boost::asio::async_read(socket_, read_buffer_, yield);
    read_buffer_.commit(read_size);
    body->write(bucket_, yield);
    bucket_.clear();
    read_buffer_.commit(0);
  }
}

void HttpConnection::handle(boost::asio::yield_context yield) {
  try {
    while (!request_parser_.ParseComplete()) {
      size_t limit = read_buffer_.max_size() - read_buffer_.size();
      size_t max_size = 65536;
      size_t read_size =
          std::min(std::max<size_t>(512, read_buffer_.capacity()),
                   std::min(limit, max_size));

      read_buffer_.prepare(read_size);
      boost::system::error_code ec;
      size_t size = boost::asio::async_read(socket_, read_buffer_, yield[ec]);
      if (ec) {
        return;
      }
      read_buffer_.commit(read_size);
      request_parser_.Parse(bucket_.data(), size);
      read_buffer_.consume(read_size);
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

  auto visitor = boost::make_overloaded_function(
      [this, &buidRequestResult,
       yield](FixedBodyHttpHandler requestHandler) mutable {
        processRequest(requestHandler, buidRequestResult.first, yield);
      },
      [this, &buidRequestResult, yield](StreamBodyHttpHandler requestHandler) {
        processRequest(requestHandler, buidRequestResult.first, yield);
      });

  boost::apply_visitor(visitor, buidRequestResult.second);
  socket_.close();
}

void HttpConnection::processRequest(FixedBodyHttpHandler handler,
                                    HttpRequest &request,
                                    boost::asio::yield_context yield) {
  HttpResponse response = handler(request, yield);
  boost::asio::async_write(
      socket_, boost::asio::buffer(response.serializeHeaders()), yield);
  boost::asio::async_write(
      socket_, boost::asio::buffer(response.serializeResponse()), yield);
}

void HttpConnection::processRequest(StreamBodyHttpHandler handler,
                                    HttpRequest &request,
                                    boost::asio::yield_context yield) {
  StreamHttpResponse response;
  boost::asio::spawn(
      socket_.get_executor(),
      [this, handler, &request, &response](boost::asio::yield_context yield) mutable {
        handler(request, response, yield);
      });

  response.awaitEndOfHeaders(yield);
  boost::asio::async_write(socket_, boost::asio::buffer(response.serializeHeaders()), yield);
  
  while (not response.isEndOfStream(yield)) {
    std::string nextChunk = response.readChunk(yield);
    boost::asio::async_write(socket_, boost::asio::buffer(nextChunk), yield);
  }
}

} // namespace empty_d::http