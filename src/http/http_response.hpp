#pragma once

#include "http/http_headers.h"
#include "http/protocol/parser/http_request_parser.hpp"
#include "http_status.h"
#include <boost/asio/spawn.hpp>
#include <boost/json.hpp>
#include <boost/variant.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>

namespace empty_d::http {

using boost::asio::ip::tcp;

class HttpConnection;

class HttpResponseBase {
public:
  [[nodiscard]] HttpStatus getStatusCode() const;
  void setStatusCode(HttpStatus status);

  [[nodiscard]] const HttpHeaders &getHeaders();

  bool setHeader(std::string name, std::string value);
  bool addHeader(std::string name, std::string value);

  bool setHeader(std::string name, std::vector<std::string> values);
  bool addHeader(std::string name, std::vector<std::string> values);

  void prepare(std::shared_ptr<HttpRequest> request);

  template <typename T> T getQueryParam() const;

  template <typename T> T getQueryParam(const T &defaultParam) const;

private:
  HttpHeaders mHeaders;
  HttpStatus mStatus;
  std::string mReason;
  std::unordered_map<std::string, std::string> mQueryParams;

  template <typename T>
  auto convertQueryParamTo(T &obj) const ->
      typename std::enable_if<(not std::is_pointer<T>::value or
                               std::is_same<T, const char *>::value),
                              bool>::type {

    return true;
  }
};

class BaseResponseBody {
public:
  friend class HttpConnection;
  virtual ~BaseResponseBody() = default;

private:
  virtual void sendResponseBody(tcp::socket &socket,
                                boost::asio::yield_context yield) = 0;
};

class FixedResponseBody : BaseResponseBody {
public:
  template <typename T>
  auto setResponseBody(T &&responseBody)
      -> std::enable_if_t<std::is_pod_v<T>, void> {}

  template <typename T>
  auto setResponseBody(T &&responseBody)
      -> std::enable_if_t<std::is_base_of_v<boost::json::value, T>, void> {}

  template <typename T>
  auto setResponseBody(T &&responseBody)
      -> std::enable_if_t<std::is_same_v<std::string, T>, void> {}

private:
  void sendResponseBody(tcp::socket &socket,
                        boost::asio::yield_context yield) override;

  std::string mResponseBody;
};

class MultipartHttpResponseBody : public BaseResponseBody {
public:
private:
  void sendResponseBody(tcp::socket &socket,
                        boost::asio::yield_context yield) override;
};

class HttpResponse : public HttpResponseBase {
public:
  void setResponseBody(std::unique_ptr<BaseResponseBody> responseBody);

  std::string serializeResponse();
  std::string serializeHeaders();

private:
  void sendResponse(tcp::socket &socket, boost::asio::yield_context yield);

  std::unique_ptr<BaseResponseBody> mResponseBody;
};

class StreamHttpResponse : public HttpResponseBase {
public:
  void sendChunk(std::string chunk, boost::asio::yield_context yield);
  std::string readChunk(boost::asio::yield_context yield);
  void setEndOfHeaders(boost::asio::yield_context yield);
  void awaitEndOfHeaders(boost::asio::yield_context yield);
  std::string serializeHeaders();
  bool isEndOfStream(boost::asio::yield_context yield);

private:
};

class HttpHandlerBase {
public:
  virtual HttpResponse handleRequest(HttpResponse &response,
                                     boost::asio::yield_context yield);

private:
};

class StreamResponseHttpHandlerBase {
public:
  virtual void handleRequest(HttpRequest &request, StreamHttpResponse &response,
                             boost::asio::yield_context yield) = 0;

private:
};

} // namespace empty_d::http
