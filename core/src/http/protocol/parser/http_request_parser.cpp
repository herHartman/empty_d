#include "http_request_parser.hpp"
#include "http/protocol/parser/http_parser/http_parser.h"
#include <cstddef>
#include <iostream>

namespace empty_d::http::protocol::parser {

const http_parser_settings HttpRequestParser::settings_ = []() {
  http_parser_settings settings{};
  settings.on_message_begin = HttpRequestParser::OnMessageBegin;
  settings.on_url = HttpRequestParser::OnUrl;
  settings.on_status = HttpRequestParser::OnStatus;
  settings.on_header_value = HttpRequestParser::OnHeaderValue;
  settings.on_header_field = HttpRequestParser::OnHeaderValue;
  settings.on_headers_complete = HttpRequestParser::OnHeadersComplete;
  settings.on_body = HttpRequestParser::OnMessageBody;
  settings.on_message_complete = HttpRequestParser::OnMessageComplete;
  settings.on_chunk_header = HttpRequestParser::OnChunkHeader;
  settings.on_chunk_complete = HttpRequestParser::OnChunkComplete;
  return settings;
}();

HttpRequestParser::HttpRequestParser() : parser_{} {
  http_parser_init(&parser_, HTTP_REQUEST);
  parser_.data = this;
}

int HttpRequestParser::OnMessageBegin(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnMessageBeginImpl(parser);
}

int HttpRequestParser::OnUrl(http_parser *parser, const char *data,
                             size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnUrlImpl(parser, data, lenght);
}

int HttpRequestParser::OnStatus(http_parser *parser, const char *data,
                                size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnStatusImpl(parser, data, lenght);
}

int HttpRequestParser::OnHeaderField(http_parser *parser, const char *data,
                                     size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnHeaderFieldImpl(parser, data, lenght);
}

int HttpRequestParser::OnHeaderValue(http_parser *parser, const char *data,
                                     size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnHeaderValueImpl(parser, data, lenght);
}

int HttpRequestParser::OnHeadersComplete(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnHeadersCompleteImpl(parser);
}

int HttpRequestParser::OnMessageBeginImpl(http_parser *parser) {}

int HttpRequestParser::OnUrlImpl(http_parser *parser, const char *data,
                                 size_t len) {
  std::cout << data << std::endl;
}

int HttpRequestParser::OnHeaderFieldImpl(http_parser *parser, const char *data,
                                         size_t len) {
  std::cout << data << std::endl;
}

int HttpRequestParser::OnHeaderValueImpl(http_parser *parser, const char *data,
                                         size_t len) {
  std::cout << data << std::endl;
}

int HttpRequestParser::OnHeadersCompleteImpl(http_parser *parser) {}

int HttpRequestParser::OnMessageBodyImpl(http_parser *parser, const char *data,
                                         size_t len) {
  std::cout << data << std::endl;
}

int HttpRequestParser::OnChunkHeaderImpl(http_parser *parser) {}

int HttpRequestParser::OnChunkCompleteImpl(http_parser *parser) {}

void HttpRequestParser::Parse() {}

} // namespace empty_d::http::protocol::parser