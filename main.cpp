#include "network/tcp_server.h"
#include "http/web/web_application.h"
#include <boost/asio/co_spawn.hpp>
#include <iostream>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

awaitable<http::http_response> handler(const http::raw_request_message& request_message) {
    co_return http::http_response({}, static_cast<http::web::http_status>(200), "", 1);
}


int main() {
    boost::asio::io_context io_context{};
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { io_context.stop(); });

    http::web::web_application application = http::web::web_application(io_context, 8080);
    application.add_route("/auth/guest", &handler, http::http_methods::HTTP_POST);

    co_spawn(io_context, application.start(), detached);

    io_context.run();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
