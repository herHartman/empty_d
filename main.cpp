// #include "network/tcp_server.h"
#include "http/http_response.hpp"
#include "http/http_status.h"
#include "http/protocol/http_methods.h"
#include "http/url_dispatcher.hpp"
#include "http/web_application.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/json.hpp>
#include <memory>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

using namespace boost;

awaitable<empty_d::http::HttpResponse>
handler(empty_d::http::request::HttpRequest &request_message) {
  co_return empty_d::http::HttpResponse(empty_d::http::HttpStatus::HTTP_OK);
}

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
  auto url_dispatcher = std::make_shared<empty_d::http::UrlDispatcher>();
  empty_d::http::WebApplication application("127.0.0.1", "8001", 10000,
                                            url_dispatcher);
  application.AddHandler(handler, empty_d::http::HttpMethods::HTTP_GET,
                         "api/v1/test");
  application.Run();
  return 0;
}
