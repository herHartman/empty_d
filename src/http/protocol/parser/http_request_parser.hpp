#pragma once

#include "http/protocol/parser/http_parser/http_parser.h"
#include "http/protocol/request/http_request.h"
#include "http/protocol/request/http_request_builder.h"

using empty_d::http::request::HttpRequestBuilder;
using empty_d::http::request::HttpRequest;

namespace empty_d::http::protocol::parser {
class HttpRequestParser {
public:
  HttpRequestParser();

  enum class ParseQueryState {
    QUERY_NAME,
    QUERY_VALUE
  };

  int OnMessageBeginImpl(http_parser *parser);
  int OnUrlImpl(http_parser *parser, const char *data, size_t lenght);
  int OnStatusImpl(http_parser *parser, const char *data, size_t lenght);
  int OnHeaderFieldImpl(http_parser *parser, const char *data, size_t lenght);
  int OnHeaderValueImpl(http_parser *parser, const char *data, size_t lenght);
  int OnHeadersCompleteImpl(http_parser *parser);
  int OnMessageBodyImpl(http_parser *parser, const char *data, size_t lenght);
  int OnMessageCompleteImpl(http_parser *parser);

  int OnChunkHeaderImpl(http_parser *parser);
  int OnChunkCompleteImpl(http_parser *parser);

  static int OnMessageBegin(http_parser *parser);
  static int OnUrl(http_parser *parser, const char *data, size_t lenght);
  static int OnStatus(http_parser *parser, const char *data, size_t lenght);
  static int OnHeaderField(http_parser *parser, const char *data,
                           size_t lenght);
  static int OnHeaderValue(http_parser *parser, const char *data,
                           size_t lenght);
  static int OnHeadersComplete(http_parser *parser);
  static int OnMessageBody(http_parser *parser, const char *data,
                           size_t lenght);
  static int OnMessageComplete(http_parser *parser);

  static int OnChunkHeader(http_parser *parser);
  static int OnChunkComplete(http_parser *parser);


  HttpRequest Parse(const char* data, size_t length);
  
private:
  static const http_parser_settings settings_;
  http_parser parser_;
  http_parser_url url_parser_;
  HttpRequestBuilder request_builder_;

  std::string_view current_header_field;
};
} // namespace empty_d::http::protocol::parser