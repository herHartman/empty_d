#pragma once

#include "http/protocol/parser/http_parser/http_parser.h"
#include <memory>

namespace empty_d::http::protocol::parser {
class HttpRequestParser final {
public:

  HttpRequestParser();

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

  void Parse();

private:
  static const http_parser_settings settings_;
  http_parser parser_;
};
} // namespace empty_d::http::protocol::parser