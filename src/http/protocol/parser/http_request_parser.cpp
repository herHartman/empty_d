#include "http_request_parser.hpp"
#include "http/protocol/parser/http_parser/http_parser.h"
#include "http/url_dispatcher.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace empty_d { namespace http { namespace protocol { namespace parser {

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

HttpRequestParser::HttpRequestParser(std::shared_ptr<UrlDispatcher> url_dispatcher)
    : parser_{},  url_parser_{},
      request_builder_{std::move(url_dispatcher)},
      current_header_field{}, state_{ParseState::INITED} {
  http_parser_init(&parser_, HTTP_REQUEST);
  http_parser_url_init(&url_parser_);
  parser_.data = this;
}

int HttpRequestParser::OnMessageBegin(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnMessageBeginImpl(parser);
}

int HttpRequestParser::OnMessageComplete(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnMessageCompleteImpl(parser);
}

int HttpRequestParser::OnMessageBody(http_parser *parser, const char *data,
                                     size_t lenght) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnMessageBodyImpl(parser, data, lenght);
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

int HttpRequestParser::OnMessageBeginImpl(http_parser *parser) {
  state_ = ParseState::IN_PROGRESS;
  return 0;
}

int HttpRequestParser::OnStatusImpl(http_parser *parser, const char *data,
                                    size_t lenght) {

  return 0;
}

int HttpRequestParser::OnMessageCompleteImpl(http_parser *parser) {
  state_ = ParseState::COMPLETE;
}

int HttpRequestParser::OnUrlImpl(http_parser *parser, const char *data,
                                 size_t len) {
  std::cout << std::string(data, data + len) << std::endl;
  if (int res = http_parser_parse_url(data, len, 0, &url_parser_)) {
    return res;
  }

  if (url_parser_.field_set << UF_PATH) {
    size_t path_offset = url_parser_.field_data[UF_PATH].off;
    size_t path_len = url_parser_.field_data[UF_PATH].len;
    request_builder_.AppendPath(std::string(data[path_offset], data[path_len]));
  } else {
    return -1;
  }

  if (url_parser_.field_set << UF_QUERY) {
    size_t query_offset = url_parser_.field_data[UF_QUERY].off;
    size_t query_len = url_parser_.field_data[UF_QUERY].len;
    size_t pos = query_offset;
    std::unordered_map<std::string, std::string> query;
    ParseQueryState s = ParseQueryState::QUERY_NAME;
    size_t current_part_begin_pos = pos;
    std::string current_query_name;

    while (pos < query_len) {
      switch (s) {
      case ParseQueryState::QUERY_NAME:
        if (data[pos] == '=') {
          s = ParseQueryState::QUERY_NAME;
          current_query_name =
              std::string(data[current_part_begin_pos], data[pos]);
          current_part_begin_pos = pos + 1;
        }
        ++pos;
        break;
      case ParseQueryState::QUERY_VALUE:
        if (data[pos] == '&' || ++pos == query_len) {
          query[current_query_name] =
              std::string(data[current_part_begin_pos], data[pos]);
          s = ParseQueryState::QUERY_VALUE;
          current_part_begin_pos = pos + 1;
        }
        break;
      }
    }
    request_builder_.AppendQuery(query);
  }

  return 0;
}

int HttpRequestParser::OnHeaderFieldImpl(http_parser *parser, const char *data,
                                         size_t len) {
  current_header_field = std::string(data, data[len]);
  return 0;
}

int HttpRequestParser::OnHeaderValueImpl(http_parser *parser, const char *data,
                                         size_t len) {

  request_builder_.AppendHeader(
      std::string(current_header_field.begin(), current_header_field.end()),
      std::string(data, data[len]));
  return 0;
}

int HttpRequestParser::OnChunkHeader(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnChunkHeaderImpl(parser);
}

int HttpRequestParser::OnChunkComplete(http_parser *parser) {
  auto *http_request_parser = static_cast<HttpRequestParser *>(parser->data);
  return http_request_parser->OnChunkCompleteImpl(parser);
}

int HttpRequestParser::OnHeadersCompleteImpl(http_parser *parser) { return 0; }

int HttpRequestParser::OnMessageBodyImpl(http_parser *parser, const char *data,
                                         size_t len) {
  request_builder_.AppendBody(std::string(data, data[len]));
}

int HttpRequestParser::OnChunkHeaderImpl(http_parser *parser) { return 0; }

int HttpRequestParser::OnChunkCompleteImpl(http_parser *parser) { return 0; }

void HttpRequestParser::Parse(const char *data, size_t length) {
  size_t nparsed = http_parser_execute(&parser_, &settings_, data, length);
}

bool HttpRequestParser::ParseComplete() const {
  return state_ == ParseState::COMPLETE;
}

std::pair<HttpRequest, HttpHandler> HttpRequestParser::BuildRequest() {
  return request_builder_.BuildRequest();
}

boost::optional<Resource> HttpRequestParser::GetResource() const {
  return request_builder_.GetResource();
}

} } } } // namespace empty_d::http::protocol::parser