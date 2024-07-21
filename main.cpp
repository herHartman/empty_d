// #include "network/tcp_server.h"
#include "http/protocol/parser/http_request_parser.hpp"
#include <boost/json.hpp>
// #include <boost/asio/co_spawn.hpp>
// #include <iostream>

// using boost::asio::awaitable;
// using boost::asio::co_spawn;
// using boost::asio::detached;
// using boost::asio::use_awaitable;
// using boost::asio::ip::tcp;
// namespace this_coro = boost::asio::this_coro;

// using namespace boost;

// awaitable<http::http_response> handler(http::request& request_message) {
//     co_return http::http_response({},
//     static_cast<http::web::http_status>(200), "", 1);
// }

using namespace empty_d::http::protocol::parser;

const char *kRawRequest =
    "GET /favicon.ico?test=tester HTTP/1.1\r\n"
    "Host: 0.0.0.0=5000\r\n"
    "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
    "Gecko/2008061015 Firefox/3.0\r\n"
    "Accept: "
    "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "Accept-Language: en-us,en;q=0.5\r\n"
    "Accept-Encoding: gzip,deflate\r\n"
    "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
    "Keep-Alive: 300\r\n"
    "Connection: keep-alive\r\n"
    "\r\n";

int main() {

  HttpRequestParser parser{};
  parser.Parse(kRawRequest, strlen(kRawRequest));
  return 0;
  // boost::asio::io_context io_context{};
  // boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
  // signals.async_wait([&](auto, auto) { io_context.stop(); });

  // http::web::web_application application =
  // http::web::web_application(io_context, 8080);
  // application.add_route("/auth/guest", &handler,
  // http::http_methods::HTTP_POST);

  // co_spawn(io_context, application.start(), detached);

  // io_context.run();

  // std::cout << "Hello, World!" << std::endl;
}
