
#include "http/http_server.h"
#include "http/http_connection.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/impl/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>

namespace empty_d::http {

void ConnectionsManager::startHandleConnection(
    std::shared_ptr<HttpConnection> connection,
    const boost::asio::any_io_executor &io) {

  auto handleConnection = [this, connection = std::move(connection)](
                              boost::asio::yield_context yield) mutable {
    mActiveConnections.insert(connection);
    try {
      connection->handle(yield);
    } catch (const std::exception &ex) {
      std::cout << "error handle http request, error was " << ex.what()
                << std::endl;
    }
    mActiveConnections.erase(connection);
  };
  
  boost::asio::spawn(boost::asio::make_strand(io), handleConnection);
}

void ConnectionsManager::stopHandleConnection(
    std::shared_ptr<HttpConnection> connection,
    boost::asio::io_context &IOContext) {}

void ConnectionsManager::stopAll() {}

void HttpServer::acceptLoop(boost::asio::yield_context yield) {
  while (mIsRunning) {
    boost::system::error_code ec;
    boost::asio::ip::tcp::socket socket{mAcceptor.get_executor()};
    mAcceptor.async_accept(socket, yield[ec]);
    if (not mAcceptor.is_open()) {
      std::cout << "Acceptor was be a closed" << std::endl;
      return;
    } else if (ec == boost::asio::error::operation_aborted) {
      std::cout << "Accept operation aborted" << std::endl;
      return;
    } else if (ec) {
      std::cout << "Accept failed: " << ec.message() << std::endl;
      if (ec == boost::system::errc::too_many_files_open ||
          ec == boost::system::errc::too_many_files_open_in_system ||
          ec == boost::system::errc::no_buffer_space ||
          ec == boost::system::errc::not_enough_memory) {
        std::cout << "Try reopen accept..." << std::endl;

        // TODO sleep async
        continue;
      }

      std::cout << "Accept was be a stopped" << std::endl;
      return;
    }
    auto connection =
        std::make_shared<HttpConnection>(mUrlDispatcher, std::move(socket));
    mConnectionsManager.startHandleConnection(std::move(connection),
                                              mAcceptor.get_executor());
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
