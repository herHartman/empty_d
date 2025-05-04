#pragma once

#include "http/http_request.h"
#include "http/http_request_builder.h"
#include "http/parser/http_parser/http_parser.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio/any_io_executor.hpp>

using empty_d::http::request::HttpRequest;
using empty_d::http::request::HttpRequestBuilder;

namespace empty_d::http::protocol::parser {
class HttpRequestParser {
public:
  HttpRequestParser(std::shared_ptr<UrlDispatcher> urlDispatcher,
                    const boost::asio::any_io_executor &executor);

  enum class ParseQueryState { QUERY_NAME, QUERY_VALUE };

  enum class ParseState { INITED, IN_PROGRESS, ERROR, COMPLETE };

  int onMessageBeginImpl(http_parser *parser);
  int onUrlImpl(http_parser *parser, const char *data, size_t lenght);
  int onStatusImpl(http_parser *parser, const char *data, size_t lenght);
  int onHeaderFieldImpl(http_parser *parser, const char *data, size_t lenght);
  int onHeaderValueImpl(http_parser *parser, const char *data, size_t lenght);
  int onHeadersCompleteImpl(http_parser *parser);
  int onMessageBodyImpl(http_parser *parser, const char *data, size_t lenght);
  int onMessageCompleteImpl(http_parser *parser);

  int onChunkHeaderImpl(http_parser *parser);
  int onChunkCompleteImpl(http_parser *parser);

  static int onMessageBegin(http_parser *parser);
  static int onUrl(http_parser *parser, const char *data, size_t lenght);
  static int onStatus(http_parser *parser, const char *data, size_t lenght);
  static int onHeaderField(http_parser *parser, const char *data,
                           size_t lenght);
  static int onHeaderValue(http_parser *parser, const char *data,
                           size_t lenght);
  static int onHeadersComplete(http_parser *parser);
  static int onMessageBody(http_parser *parser, const char *data,
                           size_t lenght);
  static int onMessageComplete(http_parser *parser);

  static int onChunkHeader(http_parser *parser);
  static int onChunkComplete(http_parser *parser);

  void parse(const char *data, size_t length);

  bool parseComplete() const;

  std::pair<HttpRequest, HttpHandler> buildRequest();

  boost::optional<Resource> getResource() const;

  HttpRequestParser(HttpRequestParser &) = delete;
  HttpRequestParser(HttpRequestParser &&) = delete;
  HttpRequestParser &operator=(const HttpRequestParser &) = delete;
  HttpRequestParser &operator=(HttpRequestParser &&) = delete;

  ~HttpRequestParser() {
    std::cout << __PRETTY_FUNCTION__ << ": was destroid, " << std::endl;
  }

private:
  static const http_parser_settings mSettings;
  http_parser mParser;
  http_parser_url mUrlParser;
  HttpRequestBuilder mRequestBuilder;
  ParseState mParseState;
  std::string mCurrentHeaderField;
};
} // namespace empty_d::http::protocol::parser