#pragma once

#include "http/http_response.hpp"
#include "http/protocol/http_connection.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_set>
#include <utility>

using boost::asio::detached;
using boost::asio::ip::tcp;

namespace empty_d::http {

class ConnectionsManager {
public:
  explicit ConnectionsManager(boost::asio::io_context &ioContext);
  void startHandleConnection(std::shared_ptr<HttpConnection> connection);
  void stopHandleConnection(std::shared_ptr<HttpConnection> connection);
  void stopAll();

private:
  std::unordered_set<std::shared_ptr<HttpConnection>> mActiveConnections;
  boost::asio::io_context &mIOContext;
  std::mutex mConnectionsMutex;
};

class HttpServer : std::enable_shared_from_this<HttpServer> {
public:
  explicit HttpServer(size_t threadPoolSize, std::string address,
                      std::string port,
                      std::shared_ptr<UrlDispatcher> urlDispatcher,
                      boost::asio::io_context &ioContext,
                      size_t maxRequests = 1000)
      : mPool(threadPoolSize),
        mAcceptor(boost::asio::make_strand(mPool.get_executor())),
        mRequestsCount(0), mIsRunning(false), mMaxRequests(maxRequests),
        mPort(std::move(port)), mUrlDispatcher(std::move(urlDispatcher)),
        mAddress(std::move(address)), mConnectionsManager(ioContext) {}

  void start();

  void shutDown();

  template <typename Handler, typename... Args>
  auto registerHandler(HttpMethods method, std::string path, Args &&...args) {
    auto handleRequest =
        [&args...](std::shared_ptr<HttpConnection> connection,
                   HttpRequest &request,
                   boost::asio::yield_context yield) mutable -> HttpResponse {
      std::unique_ptr<Handler> handler =
          std::make_unique<Handler>(std::forward<Args>(args)...);
      connection->processRequest(handler, request, yield);
    };
    mUrlDispatcher->addHandler(handleRequest, method, path);
  }

private:
  void acceptLoop(boost::asio::yield_context yield);

  boost::asio::thread_pool mPool;
  tcp::acceptor mAcceptor;
  std::size_t mRequestsCount;
  std::atomic<bool> mIsRunning{false};
  std::size_t mMaxRequests;
  std::string mAddress;
  std::string mPort;
  std::shared_ptr<UrlDispatcher> mUrlDispatcher;
  ConnectionsManager mConnectionsManager;
};
} // namespace empty_d::http
