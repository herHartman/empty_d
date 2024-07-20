#pragma once

#include <boost/asio.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/detail/error_code.hpp>
#include <string>
#include <string_view>

using boost::asio::ip::tcp;

namespace empty_d::redis {

template <typename Executor> class Request {
  using executor_type = Executor;
  using receive_channel_type = boost::asio::experimental::channel<
      executor_type, void(boost::system::error_code, std::size_t)>;
  using exec_notifier_type = receive_channel_type;

  void Set(std::string_view key);
  void Get(std::string_view key);
  void Save();

private:
  std::string payload_;
  exec_notifier_type notifier_;
};

class Response {};

template <typename Executor> class BaseRedisConnection {
public:
  using executor_type = Executor;

  using clock_type = std::chrono::steady_clock;
  using clock_traits_type = boost::asio::wait_traits<clock_type>;
  using timer_type =
      boost::asio::basic_waitable_timer<clock_type, clock_traits_type,
                                        executor_type>;

  explicit BaseRedisConnection(executor_type &io_context,
                               const std::string &address, std::size_t port)
      : io_context_(io_context), socket_(io_context),
        redis_server_endpoint_(boost::asio::ip::address::from_string(address),
                               port),
        writer_timer_{io_context} {
    writer_timer_.expires_at((std::chrono::steady_clock::time_point::max)());
  }

  [[nodiscard]] bool IsConnected() const;

  auto Connect() -> boost::asio::awaitable<void> {
    if (IsConnected())
      co_return;
    boost::system::error_code ec = co_await ConnectImpl();
  }

  auto AsyncWrite() -> boost::asio::awaitable<void> {
    for (;;) {

      // wait for requests
      co_await writer_timer_.async_wait(boost::asio::use_awaitable);
    }
  }

  auto AsyncExecuteCommand(const Request &req) -> boost::asio::awaitable<void> {
    requests_.push_back(req);
  }

  auto AsyncRead() -> boost::asio::awaitable<void> {}

  void Disconnect();

  executor_type &get_executor() { return io_context_; }

private:
  executor_type &io_context_;
  tcp::socket socket_;
  tcp::endpoint redis_server_endpoint_;
  std::vector<Request> requests_;
  timer_type writer_timer_;

  using dyn_buffer_type =
      boost::asio::dynamic_string_buffer<char, std::char_traits<char>,
                                         std::allocator<char>>;

  std::string read_buffer_;
  std::string write_buffer_;
  dyn_buffer_type dynamic_buffer_;

  auto ConnectImpl() -> boost::asio::awaitable<boost::system::error_code> {
    socket_.open(tcp::v4());
    auto [error_code] = co_await socket_.async_connect(
        redis_server_endpoint_,
        boost::asio::as_tuple(boost::asio::use_awaitable));
    co_return error_code;
  }
};
} // namespace empty_d::redis