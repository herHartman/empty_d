#include "http_request_parser.hpp"
#include "http/http_methods.h"
#include "http/parser/http_parser/http_parser.h"
#include "http/url_dispatcher.hpp"
#include <boost/asio/any_io_executor.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace empty_d::http::protocol::parser {

const http_parser_settings HttpRequestParser::mSettings = []() {
  http_parser_settings settings{};
  settings.on_message_begin = HttpRequestParser::onMessageBegin;
  settings.on_url = HttpRequestParser::onUrl;
  settings.on_status = HttpRequestParser::onStatus;
  settings.on_header_value = HttpRequestParser::onHeaderValue;
  settings.on_header_field = HttpRequestParser::onHeaderField;
  settings.on_headers_complete = HttpRequestParser::onHeadersComplete;
  settings.on_body = HttpRequestParser::onMessageBody;
  settings.on_message_complete = HttpRequestParser::onMessageComplete;
  settings.on_chunk_header = HttpRequestParser::onChunkHeader;
  settings.on_chunk_complete = HttpRequestParser::onChunkComplete;
  return settings;
}();

HttpRequestParser::HttpRequestParser(
    std::shared_ptr<UrlDispatcher> urlDispatcher,
    const boost::asio::any_io_executor &executor)
    : mParser{}, mUrlParser{},
      mRequestBuilder(std::move(urlDispatcher), executor),
      mCurrentHeaderField{}, mParseState{ParseState::INITED} {
  http_parser_init(&mParser, HTTP_REQUEST);
  http_parser_url_init(&mUrlParser);
  mParser.data = this;
}

int HttpRequestParser::onMessageBegin(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onMessageBeginImpl(parser);
}

int HttpRequestParser::onMessageComplete(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onMessageCompleteImpl(parser);
}

int HttpRequestParser::onMessageBody(http_parser *parser, const char *data,
                                     size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onMessageBodyImpl(parser, data, lenght);
}

int HttpRequestParser::onUrl(http_parser *parser, const char *data,
                             size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onUrlImpl(parser, data, lenght);
}

int HttpRequestParser::onStatus(http_parser *parser, const char *data,
                                size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onStatusImpl(parser, data, lenght);
}

int HttpRequestParser::onHeaderField(http_parser *parser, const char *data,
                                     size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onHeaderFieldImpl(parser, data, lenght);
}

int HttpRequestParser::onHeaderValue(http_parser *parser, const char *data,
                                     size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onHeaderValueImpl(parser, data, lenght);
}

int HttpRequestParser::onHeadersComplete(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onHeadersCompleteImpl(parser);
}

int HttpRequestParser::onMessageBeginImpl(http_parser *parser) {
  mParseState = ParseState::IN_PROGRESS;
  switch (parser->method) {
  case http_method::HTTP_DELETE:
    break;
  case http_method::HTTP_GET:
  mRequestBuilder.AppendMethod(HttpMethods::HTTP_GET);
    break;
  case http_method::HTTP_HEAD:
    break;
  case http_method::HTTP_POST:
    mRequestBuilder.AppendMethod(HttpMethods::HTTP_POST);
    break;
  case http_method::HTTP_PUT:
    break;
  case http_method::HTTP_CONNECT:
    break;
  case http_method::HTTP_OPTIONS:
    break;
  case http_method::HTTP_TRACE:
    break;
  case http_method::HTTP_COPY:
    break;
  case http_method::HTTP_LOCK:
    break;
  case http_method::HTTP_MKCOL:
    break;
  case http_method::HTTP_MOVE:
    break;
  case http_method::HTTP_PROPFIND:
    break;
  case http_method::HTTP_PROPPATCH:
    break;
  case http_method::HTTP_SEARCH:
    break;
  case http_method::HTTP_UNLOCK:
    break;
  case http_method::HTTP_BIND:
    break;
  case http_method::HTTP_REBIND:
    break;
  case http_method::HTTP_UNBIND:
    break;
  case http_method::HTTP_REPORT:
    break;
  case http_method::HTTP_MKACTIVITY:
    break;
  case http_method::HTTP_CHECKOUT:
    break;
  case http_method::HTTP_MERGE:
    break;
  case http_method::HTTP_MSEARCH:
    break;
  case http_method::HTTP_NOTIFY:
    break;
  case http_method::HTTP_SUBSCRIBE:
    break;
  case http_method::HTTP_UNSUBSCRIBE:
    break;
  case http_method::HTTP_PATCH:
    break;
  case http_method::HTTP_PURGE:
    break;
  case http_method::HTTP_MKCALENDAR:
    break;
  case http_method::HTTP_LINK:
    break;
  case http_method::HTTP_UNLINK:
    break;
  case http_method::HTTP_SOURCE:
    break;
  }

  mRequestBuilder.AppendHttpVersion(std::to_string(parser->http_major));
  return 0;
}

int HttpRequestParser::onStatusImpl(http_parser *parser, const char *data,
                                    size_t lenght) {

  return 0;
}

int HttpRequestParser::onMessageCompleteImpl(http_parser *parser) {
  mParseState = ParseState::COMPLETE;
  return 0;
}

int HttpRequestParser::onUrlImpl(http_parser *parser, const char *data,
                                 size_t len) {
  std::cout << std::string(data, data + len) << std::endl;
  if (int res = http_parser_parse_url(data, len, 0, &mUrlParser)) {
    return res;
  }

  if (mUrlParser.field_set << UF_PATH) {
    size_t path_offset = mUrlParser.field_data[UF_PATH].off;
    size_t path_len = mUrlParser.field_data[UF_PATH].len;
    mRequestBuilder.AppendPath(
        std::string(data + path_offset, data + path_len));
  } else {
    return -1;
  }

  if (mUrlParser.field_set << UF_QUERY) {
    size_t query_offset = mUrlParser.field_data[UF_QUERY].off;
    size_t query_len = mUrlParser.field_data[UF_QUERY].len;
    size_t pos = query_offset;
    std::unordered_map<std::string, std::string> query;
    ParseQueryState s = ParseQueryState::QUERY_NAME;
    size_t current_part_begin_pos = pos;
    std::string current_query_name;

    while (pos < query_offset + query_len) {
      switch (s) {
      case ParseQueryState::QUERY_NAME:
        if (data[pos] == '=') {
          s = ParseQueryState::QUERY_VALUE;
          current_query_name =
              std::string(data + current_part_begin_pos, data + pos);
          current_part_begin_pos = pos + 1;
        }
        ++pos;
        break;
      case ParseQueryState::QUERY_VALUE:
        if (data[pos] == '&' || ++pos == query_offset + query_len) {
          query[current_query_name] =
              std::string(data + current_part_begin_pos, data + pos);
          s = ParseQueryState::QUERY_NAME;
          current_part_begin_pos = pos + 1;
        }
        break;
      }
    }
    mRequestBuilder.AppendQuery(query);
  }
  return 0;
}

int HttpRequestParser::onHeaderFieldImpl(http_parser *parser, const char *data,
                                         size_t len) {
  mCurrentHeaderField = std::string(data, data + len);
  return 0;
}

int HttpRequestParser::onHeaderValueImpl(http_parser *parser, const char *data,
                                         size_t len) {
  mRequestBuilder.AppendHeader(
      std::string(mCurrentHeaderField.begin(), mCurrentHeaderField.end()),
      std::string(data, data + len));
  return 0;
}

int HttpRequestParser::onChunkHeader(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onChunkHeaderImpl(parser);
}

int HttpRequestParser::onChunkComplete(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->onChunkCompleteImpl(parser);
}

int HttpRequestParser::onHeadersCompleteImpl(http_parser *parser) {
  mParseState = ParseState::HEADERS_COMPLETE;
  return 0; 
}

int HttpRequestParser::onMessageBodyImpl(http_parser *parser, const char *data,
                                         size_t len) {
  mRequestBuilder.AppendBody(std::string(data, data + len));
  return 0;
}

int HttpRequestParser::onChunkHeaderImpl(http_parser *parser) { return 0; }

int HttpRequestParser::onChunkCompleteImpl(http_parser *parser) { return 0; }

void HttpRequestParser::parse(const char *data, size_t length) {
  size_t nparsed = http_parser_execute(&mParser, &mSettings, data, length);
}

bool HttpRequestParser::parseComplete() const {
  return mParseState == ParseState::COMPLETE;
}

std::pair<HttpRequest, HttpHandler> HttpRequestParser::buildRequest() {
  return mRequestBuilder.BuildRequest();
}

boost::optional<Resource> HttpRequestParser::getResource() const {
  return mRequestBuilder.GetResource();
}

} // namespace empty_d::http::protocol::parser