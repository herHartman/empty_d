#include "http/protocol/http_connection.h"
#include "http/http_response.hpp"
#include "http/url_dispatcher.hpp"
#include <algorithm>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <stdexcept>

namespace empty_d::http {

awaitable<void> HttpConnection::HandleRequest(HttpRequest &request) {
  std::optional<Resource> resource = request_parser_.GetResource();
  if (!resource) {
    throw std::runtime_error("resource is required");
  }

  HttpHandler handler = resource->GetHandler(request.GetMethod());
  HttpResponse response = co_await handler(request);
}

awaitable<void> HttpConnection::Handle() {
  try {
    while (!request_parser_.ParseComplete()) {
      size_t limit = read_buffer_.max_size() - read_buffer_.size();
      size_t max_size = 65536;
      size_t read_size =
          std::min(std::max<size_t>(512, read_buffer_.capacity()),
                   std::min(limit, max_size));

      read_buffer_.prepare(read_size);
      auto [ec, size] = co_await boost::asio::async_read(
          socket_, read_buffer_,
          boost::asio::as_tuple(boost::asio::use_awaitable));
      if (ec) {
        co_return;
      }
      read_buffer_.commit(read_size);
      request_parser_.Parse(bucket_.data(), size);
      read_buffer_.consume(read_size);
    }
  } catch (std::exception &e) {
    std::printf("echo Exception: %s\n", e.what());
  }

  HttpRequest request = request_parser_.BuildRequest();

  std::shared_ptr<request::HttpBodyStreamReader> body_reader =
      request.GetStreamReader();

  while (!body_reader->IsEof()) {
    size_t limit = read_buffer_.max_size() - read_buffer_.size();
    size_t max_size = 65536;
    size_t read_size = std::min(std::max<size_t>(512, read_buffer_.capacity()),
                                std::min(limit, max_size));

    read_buffer_.prepare(read_size);
    auto [ec, size] = co_await boost::asio::async_read(
        socket_, read_buffer_,
        boost::asio::as_tuple(boost::asio::use_awaitable));
    read_buffer_.commit(read_size);

    co_await body_reader->Write(std::move(bucket_));
    read_buffer_.commit(read_size);
  }

  co_await response_awaiter_.async_receive();
  // тут дожидаемся ответа
}

} // namespace empty_d::http