#pragma once

#include "http/http_connection.h"
#include "http/http_response.hpp"
#include "http/url_dispatcher.hpp"
#include <boost/asio.hpp>
#include <boost/asio/any_io_executor.hpp>
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
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <utility>

using boost::asio::detached;
using boost::asio::ip::tcp;

namespace empty_d::http {

constexpr uint64_t kDefaultRequestTimeout = 3600;
constexpr uint64_t kMaxHeaderSize = 16 * 1024;

class ConnectionsManager {
public:
  void startHandleConnection(std::shared_ptr<HttpConnection> connection,
                             const boost::asio::any_io_executor &io);
  void stopHandleConnection(std::shared_ptr<HttpConnection> connection,
                            boost::asio::io_context &IOContext);
  void stopAll();

private:
  std::unordered_set<std::shared_ptr<HttpConnection>> mActiveConnections;
  std::mutex mConnectionsMutex;
};

class HttpServer : std::enable_shared_from_this<HttpServer> {
public:
  HttpServer(size_t threadPoolSize, std::string address, std::string port,
             std::shared_ptr<UrlDispatcher> urlDispatcher,
             size_t maxRequests = 1000,
             uint64_t requestTimeout = kDefaultRequestTimeout,
             uint64_t maxHeaderSize = kMaxHeaderSize)
      : mPool(threadPoolSize),
        mAcceptor(boost::asio::make_strand(mPool.get_executor())),
        mRequestsCount(0), mIsRunning(false), mMaxRequests(maxRequests),
        mPort(std::move(port)), mUrlDispatcher(std::move(urlDispatcher)),
        mAddress(std::move(address)), mRequestTimeout(requestTimeout),
        mMaxHeaderSize(maxHeaderSize) {}

  void start();

  void shutDown();

  template <typename Handler, typename... Args>
  auto registerHandler(HttpMethods method, std::string path, Args &&...args) {
    auto handleRequest =
        [&args...](std::shared_ptr<HttpConnection> connection,
                   HttpRequest &request,
                   boost::asio::yield_context yield) mutable -> void {
      std::unique_ptr<Handler> handler =
          std::make_unique<Handler>(std::forward<Args>(args)...);
      connection->processRequest(std::move(handler), request, yield);
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
  uint64_t mRequestTimeout;
  uint64_t mMaxHeaderSize;
  ConnectionsManager mConnectionsManager;
};
} // namespace empty_d::http
