
#include "http/http_server.h"
#include "http/protocol/http_connection.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <exception>
#include <mutex>


namespace empty_d::http {

ConnectionsManager::ConnectionsManager(boost::asio::io_context &ioContext)
    : mIOContext(ioContext), mActiveConnections{} {}

void ConnectionsManager::startHandleConnection(
    std::shared_ptr<HttpConnection> connection) {
  {
    std::lock_guard<std::mutex> lock{mConnectionsMutex};
    mActiveConnections.insert(connection);
  }

  boost::asio::spawn(mIOContext,
                     [this, connection](boost::asio::yield_context yield) {
                       try {
                         connection->handle(yield);
                       } catch (const std::exception &ex) {
                         std::cout << "error handle http request, error was "
                                   << ex.what() << std::endl;
                       }
                       {
                         std::lock_guard<std::mutex> lock{mConnectionsMutex};
                         mActiveConnections.erase(connection);
                       }
                     });
}

void ConnectionsManager::stopHandleConnection(
    std::shared_ptr<HttpConnection> connection) {}

void ConnectionsManager::stopAll() {}

void HttpServer::acceptLoop(boost::asio::yield_context yield) {
  while (mIsRunning) {
    auto socket = mAcceptor.async_accept(yield);
    auto connection = std::make_shared<HttpConnection>(
        protocol::parser::HttpRequestParser{mUrlDispatcher}, std::move(socket));
    mConnectionsManager.startHandleConnection(connection);
  }
}

void HttpServer::start() {
  boost::asio::ip::tcp::resolver resolver(mAcceptor.get_executor());
  boost::asio::ip::tcp::endpoint endpoint =
      *resolver.resolve(mAddress, mPort).begin();
  mAcceptor.open(endpoint.protocol());
  mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  mAcceptor.bind(endpoint);
  mAcceptor.listen();
  mIsRunning = true;

  boost::asio::spawn(
      mAcceptor.get_executor(),
      [this](boost::asio::yield_context yield) { acceptLoop(yield); });

  mPool.join();
}

void HttpServer::shutDown() {
  mIsRunning.store(false);
  mAcceptor.cancel();
  mPool.stop();
  mPool.join();
}

} // namespace empty_d::http

