
#include "http/http_server.h"
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace empty_d::http {
auto HttpServer::Start() {
  boost::asio::ip::tcp::resolver resolver(acceptor_.get_executor());
  boost::asio::ip::tcp::endpoint endpoint =
      *resolver.resolve(address_, port_).begin();
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  is_running_ = true;
  
  
}

void HttpServer::ShutDown() {
  acceptor_.close();
  is_running_ = false;
}

void HttpServer::AddNewConnection(std::shared_ptr<HttpConnection> connection) {
  auto uuid = boost::uuids::random_generator()();
  http_connections_[uuid] = std::move(connection);
}

} // namespace empty_d::http
