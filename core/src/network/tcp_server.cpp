//
// Created by chopk on 19.03.24.
//

#include "tcp_server.h"
#include <boost/asio/awaitable.hpp>

awaitable<void> network::tcp_server::start() {
  is_running_ = true;
  while (is_running_) {
    auto connection =
        protocol_factory_.create_protocol(std::make_shared<transport>(
            co_await acceptor_.async_accept(boost::asio::use_awaitable)));

    co_spawn(acceptor_.get_executor(), handle_connection(std::move(connection)),
             detached);

    ++requests_count_;
  }
}

void network::tcp_server::shutdown() {
  acceptor_.close();
  is_running_ = false;
}
