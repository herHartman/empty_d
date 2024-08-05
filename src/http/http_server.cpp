
#include "http/http_server.h"
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace empty_d::http {


void HttpServer::ShutDown() {
  acceptor_.close();
  is_running_ = false;
}

void HttpServer::AddNewConnection(std::shared_ptr<HttpConnection> connection) {
  auto uuid = boost::uuids::random_generator()();
  http_connections_[uuid] = std::move(connection);
}

} // namespace empty_d::http
