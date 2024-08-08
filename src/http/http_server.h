#pragma once

#include "http/protocol/http_connection.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <memory>
#include <unordered_map>
#include <utility>

using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;
using boost::asio::detached;
namespace uuids = boost::uuids;

namespace empty_d::http {

using HttpConnections =
    std::unordered_map<uuids::uuid, std::shared_ptr<HttpConnection>,
                       boost::hash<boost::uuids::uuid>>;

class HttpServer {
public:
  explicit HttpServer(boost::asio::io_context &io_context, std::string address,
                      std::string port,
                      std::shared_ptr<UrlDispatcher> url_dispatcher,
                      std::size_t max_requests = 1000)
      : acceptor_(io_context), requests_count_(0), is_running_(false),
        max_requests_(max_requests), port_(std::move(port)),
        url_dispatcher_(std::move(url_dispatcher)),
        address_(std::move(address)),
        uuid_generator_(new uuids::random_generator()) {}

  template <typename CompletionToken> auto Start(CompletionToken &&token) {
    boost::asio::ip::tcp::resolver resolver(acceptor_.get_executor());
    boost::asio::ip::tcp::endpoint endpoint =
        *resolver.resolve(address_, port_).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    is_running_ = true;

    return boost::asio::async_compose<CompletionToken, void()>(
        [&](auto &self) -> boost::asio::awaitable<void> {
          for (;;) {
            auto socket =
                co_await acceptor_.async_accept(boost::asio::use_awaitable);
            auto new_connection = std::make_shared<HttpConnection>(
                protocol::parser::HttpRequestParser(url_dispatcher_), std::move(socket));
            AddNewConnection(new_connection);
            boost::asio::co_spawn(acceptor_.get_executor(),
                                  new_connection->Handle(),
                                  boost::asio::detached);
          }
          self.complete();
        },
        std::forward<CompletionToken>(token), acceptor_);
  }
  void ShutDown();

private:
  tcp::acceptor acceptor_;
  std::size_t requests_count_;
  bool is_running_;
  std::size_t max_requests_;
  std::string address_;
  std::string port_;
  std::unique_ptr<uuids::random_generator> uuid_generator_;
  HttpConnections http_connections_{};
  std::shared_ptr<UrlDispatcher> url_dispatcher_;

  void AddNewConnection(std::shared_ptr<HttpConnection> connection);
};
} // namespace empty_d::http
