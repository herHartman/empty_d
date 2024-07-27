#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include "http/protocol/parser/http_request_parser.hpp"
#include "http/protocol/request/http_request_parser.h"

using namespace empty_d::http::protocol::parser;

const char* kRawRequest = "GET /favicon.ico HTTP/1.1\r\n"
         "Host: 0.0.0.0=5000\r\n"
         "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"
         "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
         "Accept-Language: en-us,en;q=0.5\r\n"
         "Accept-Encoding: gzip,deflate\r\n"
         "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
         "Keep-Alive: 300\r\n"
         "Connection: keep-alive\r\n"
         "\r\n";

TEST(HttpParserTest, TestCallbacks) {
  // HttpRequestParser parser{};
  // parser.Parse(kRawRequest, strlen(kRawRequest));
  http::headers_parser();
}