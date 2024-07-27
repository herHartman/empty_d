#include "http/protocol/http_connection.h"
#include <boost/asio/use_awaitable.hpp>

namespace empty_d::http {
awaitable<void> HttpConnection::Handle() {
  try {
    auto [ec, size] = co_await boost::asio::async_read(
        socket_, read_buffer_, boost::asio::transfer_exactly(1024),
        boost::asio::as_tuple(boost::asio::use_awaitable));

    if (ec) {
      co_return;
    }

    HttpRequest request = request_parser_.Parse(bucket_.data(), size);
    
    

  } catch (std::exception &e) {
    std::printf("echo Exception: %s\n", e.what());
  }
}
} // namespace empty_d::http