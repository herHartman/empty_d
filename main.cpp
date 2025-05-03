#include "http/http_methods.h"
#include "http/http_response.hpp"
#include "http/http_server.h"
#include "http/http_status.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/assert/source_location.hpp>
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <memory>

using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

using namespace boost;

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

struct PaginationParams {
  uint32_t perPage{0};
  uint32_t pageNumber{0};
};

struct AuthCredentionals {
  std::string username;
  std::string password;
};

BOOST_DESCRIBE_STRUCT(AuthCredentionals, (), (username, password))

BOOST_DESCRIBE_STRUCT(PaginationParams, (), (perPage, pageNumber))

struct GetFileListingRequest {
  AuthCredentionals authCredentionals;
  PaginationParams pagination;
  std::string repositoryCreatedStartDate;
  std::string repositoryCreatedEndDate;
  std::string resourceName;
};

BOOST_DESCRIBE_STRUCT(GetFileListingRequest, (),
                      (authCredentionals, pagination,
                       repositoryCreatedStartDate, repositoryCreatedEndDate,
                       resourceName))

const static char *kUsernameArg = "username";

class TestHandler : public empty_d::http::HttpHandlerBase {

public:
  auto handleRequest(empty_d::http::request::HttpRequest &request,
                     boost::asio::yield_context yield)
      -> empty_d::http::HttpResponse override {
    
    auto useranme = request.getQueryArg<std::string>(kUsernameArg);

    auto requestBody = request.ReadBody<GetFileListingRequest>(yield);
    // тут какая-то очень сложная логика

    empty_d::http::HttpResponse response;
    response.setStatus(empty_d::http::HttpStatus::HTTP_OK);
    auto responseBody = std::make_unique<empty_d::http::TextResponseBody>();
    responseBody->setBody(std::move(requestBody));
    return response;
  }
};


int main() {
  auto url_dispatcher = std::make_shared<empty_d::http::UrlDispatcher>();
  std::shared_ptr<empty_d::http::HttpServer> server =
      std::make_shared<empty_d::http::HttpServer>(16, "127.0.0.1", "8001",
                                                  url_dispatcher);
  server->registerHandler<TestHandler>(empty_d::http::HttpMethods::HTTP_POST,
                                       "/api/v1/test_path");
  server->start();

  return 0;
}
