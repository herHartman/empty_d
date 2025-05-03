#pragma once

#include "definitions/strings/json_serialization.h"
#include "http/http_headers.h"
#include "http/http_request.h"
#include "http_status.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/json.hpp>
#include <boost/json/fwd.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_from.hpp>
#include <boost/variant.hpp>
#include <string>
#include <type_traits>
#include <utility>

namespace boost::json {
struct value_from_tag;
}

namespace empty_d::http {

class HttpConnection;

class BaseResponseBody {
public:
  friend class HttpConnection;
  virtual ~BaseResponseBody() = default;

  [[nodiscard]] virtual HttpHeaders buildSpecificBodyHeaders() const = 0;
  virtual void sendBody(tcp::socket &socket,
                        boost::asio::yield_context yield) = 0;
};

class TextResponseBody : public BaseResponseBody {
public:
  template <typename T>
  auto setBody(T &&responseBody)
      -> std::enable_if_t<
          empty_d::json_serialization::has_tag_invoke_value_from_v<T>, void>;

  template <typename T>
  auto setBody(T &&responseBody)
      -> std::enable_if_t<std::is_base_of_v<boost::json::value, T>, void>;

  template <typename T>
  auto setBody(T &&responseBody)
      -> std::enable_if_t<std::is_same_v<std::string, T>, void>;

  HttpHeaders buildSpecificBodyHeaders() const override;
  void sendBody(tcp::socket &socket, boost::asio::yield_context yield) override;

private:
  std::string mResponseBody;
  std::string mContentType;
};

class MultipartResponseBody : public BaseResponseBody {
public:
private:
  void sendBody(tcp::socket &socket, boost::asio::yield_context yield) override;
};

class FileResponseBody : public BaseResponseBody {};

class StreamResponseBody : public BaseResponseBody {
public:
  using StreamGenerator =
      std::function<void(boost::asio::yield_context, StreamResponseBody &)>;
  // void sendChunk(std::string chunk, boost::asio::yield_context yield);
  // std::string readChunk(boost::asio::yield_context yield);
  // bool isEndOfStream(boost::asio::yield_context yield);
  void sendBody(tcp::socket &socket, boost::asio::yield_context yield) override;

private:
};

class HttpResponse {
public:
  void setBody(std::unique_ptr<BaseResponseBody> body);

  HttpStatus getStatus() const;
  void setStatus(HttpStatus status);

  const HttpHeaders &getHeaders() const;

  bool setHeader(const std::string &name, std::string value);
  bool addHeader(const std::string &name, std::string value);

  bool setHeaders(const std::string &name, std::vector<std::string> values);
  bool addHeaders(const std::string &name, std::vector<std::string> values);

  void setContentType(std::string contentType);
  const std::string &getContentType() const;

  void setContentSize(size_t contentSize);
  size_t getContentSize() const;

  void setHeaders(HttpHeaders headers);
  void addHeaders(HttpHeaders headers);

  void sendResponse(tcp::socket &socket, boost::asio::yield_context yield);

private:
  void prepareHeaders();

  std::unique_ptr<BaseResponseBody> mResponseBody;
  HttpStatus mStatus;
  HttpHeaders mHeaders;
};

template <typename T>
auto TextResponseBody::setBody(T &&responseBody)
    -> std::enable_if_t<
        empty_d::json_serialization::has_tag_invoke_value_from_v<T>, void> {
  std::string responseBodyJson = boost::json::value_to<std::string>(
      boost::json::value_from<T>(std::forward<T>(responseBody)));

  mContentType = "application/json";
  mResponseBody = std::move(responseBodyJson);
}

template <typename T>
auto TextResponseBody::setBody(T &&responseBody)
    -> std::enable_if_t<std::is_base_of_v<boost::json::value, T>, void> {
  std::string responseBodyJson =
      boost::json::value_to<std::string>(std::forward<T>(responseBody));

  mContentType = "application/json";
  mResponseBody = std::move(responseBodyJson);
}

template <typename T>
auto TextResponseBody::setBody(T &&responseBody)
    -> std::enable_if_t<std::is_same_v<std::string, T>, void> {
  
  mResponseBody = std::forward<T>(responseBody);
  mContentType = "text/plain";
}

class HttpHandlerBase {
public:
  virtual HttpResponse
  handleRequest(empty_d::http::request::HttpRequest &request,
                boost::asio::yield_context yield) = 0;
};

class StreamResponseHttpHandlerBase {
public:
  virtual void handleRequest(empty_d::http::request::HttpRequest &request,
                             StreamResponseBody &response,
                             boost::asio::yield_context yield) = 0;
};

} // namespace empty_d::http
