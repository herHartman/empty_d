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
                      std::size_t maxRequests = 1000)
      : mPool(threadPoolSize),
        mAcceptor(boost::asio::make_strand(mPool.get_executor())),
        mRequestsCount(0), mIsRunning(false), mMaxRequests(maxRequests),
        mPort(std::move(port)), mUrlDispatcher(std::move(urlDispatcher)),
        mAddress(std::move(address)) {}

  void start();

  void shutDown();

  template <typename Handler, typename... Args>
  auto registerHandler(HttpMethods method, std::string path, Args &&...ctorArgs)
      -> std::enable_if_t<std::is_base_of_v<BaseHttpRequestHandler, Handler>,
                          void> {
    auto handleRequest = [](HttpRequest &request,
                            boost::asio::yield_context yield) mutable {
      Handler handler(std::forward<Args>...);
      return handler.handleRequest(request, yield);
    };
    mUrlDispatcher->addHandler(std::move(handleRequest), method,
                               std::move(path));
  }

  template <typename Handler, typename... Args>
  auto registerHandler(HttpMethods method, std::string path, Args &&...ctorArgs)
      -> std::enable_if_t<
          std::is_base_of_v<BaseHttpStreamRequestHandler, Handler>, void> {
    auto handleRequest = [](HttpRequest &request, HttpStreamResponse &response,
                            boost::asio::yield_context yield) {
      Handler handler(std::forward<Args>...);
      return handler.handleRequest(request, response, yield);
    };
    mUrlDispatcher->addHandler(std::move(handleRequest), method,
                               std::move(path));
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
