#pragma once

#include "http/http_headers.h"
#include "http/protocol/parser/http_request_parser.hpp"
#include "http_status.h"
#include <boost/asio/spawn.hpp>

namespace empty_d {
namespace http {

enum class ResponseType { None, Text, File, Chunked };

class HttpWriter {
public:
  void enableChunked();
  void enableCompression();
  void write(const std::string &buffer);
  void writeLines(const std::vector<std::string> &buffers);
  void setEof();
  void writeEof();

private:
  size_t mLenght;
  bool mChunked;
  bool mEof;
  bool mCompression;
  // здесь будет транспорт
};

class HttpResponseBody {
public:
  virtual ~HttpResponseBody() = default;

  virtual void write(boost::asio::yield_context yield) = 0;
  virtual void prepareHeaders(HttpHeaders &headers) = 0;
  virtual std::shared_ptr<HttpResponseBody> clone() = 0;
};

class StreamBody : public HttpResponseBody {
public:
  
private:
};

class HttpResponse {
public:
  explicit HttpResponse(HttpStatus status) : status_(status) {};

  std::string serializeResponse();

  [[nodiscard]] HttpStatus getStatusCode() const;
  void SetStatucCode(HttpStatus status);

  [[nodiscard]] HttpHeaders &getHeaders();

  bool setHeader(std::string name, std::string value);

  void prepare(std::shared_ptr<HttpRequest> request);

private:
  HttpHeaders headers_;
  std::string body_;
  HttpStatus status_;
  std::string reason_;
};
} // namespace http
} // namespace empty_d