#include "http_response.hpp"
#include "http/http_headers.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <memory>
#include <string>

namespace empty_d::http {

void TextResponseBody::sendBody(tcp::socket &socket,
                                boost::asio::yield_context yield) {
  boost::asio::socket_base::send_buffer_size option;
  socket.get_option(option);
  const size_t chunkSize = option.value();
  size_t remaining = mResponseBody.size();
  const char *dataPtr = mResponseBody.c_str();

  while (remaining > 0 && socket.is_open()) {
    size_t sendSize = std::min(remaining, chunkSize);

    size_t bytesTransferred = boost::asio::async_write(
        socket, boost::asio::buffer(dataPtr, sendSize), yield);

    dataPtr += bytesTransferred;
    remaining -= bytesTransferred;
  }
}

HttpHeaders TextResponseBody::buildSpecificBodyHeaders() const {
  HttpHeaders result;
  result.setContentLength(mResponseBody.length());
  result.setContentType(mContentType);
  return result;
}

bool HttpResponse::addHeader(const std::string &name, std::string value) {
  mHeaders.add(name, std::move(value));
  return true;
}

HttpStatus HttpResponse::getStatus() const { return mStatus; }

void HttpResponse::setBody(std::unique_ptr<BaseResponseBody> responseBody) {
  mResponseBody = std::move(responseBody);
}

void HttpResponse::setStatus(HttpStatus status) { mStatus = status; }

const HttpHeaders &HttpResponse::getHeaders() const { return mHeaders; }

bool HttpResponse::setHeader(const std::string &name, std::string value) {
  mHeaders.set(name, std::move(value));
  return true;
}

bool HttpResponse::setHeaders(const std::string &name,
                              std::vector<std::string> values) {
  mHeaders.set(name, std::move(values));
  return true;
}

bool HttpResponse::addHeaders(const std::string &name,
                              std::vector<std::string> values) {
  mHeaders.add(name, std::move(values));
  return true;
}

void HttpResponse::setContentType(std::string contentType) {
  mHeaders.setContentType(std::move(contentType));
}

const std::string &HttpResponse::getContentType() const {
  return mHeaders.getContentType();
}

void HttpResponse::setContentSize(size_t contentSize) {
  mHeaders.setContentLength(contentSize);
}

size_t HttpResponse::getContentSize() const {
  return mHeaders.getContentLength();
}

void HttpResponse::setHeaders(HttpHeaders headers) {
  mHeaders = std::move(headers);
}

void HttpResponse::addHeaders(HttpHeaders headers) {
  mHeaders = std::move(headers);
}

void HttpResponse::prepareHeaders() {
  if (mResponseBody) {
    mHeaders.addHeaders(mResponseBody->buildSpecificBodyHeaders());
  }
}

void HttpResponse::sendResponse(tcp::socket &socket,
                                boost::asio::yield_context yield)

{
  prepareHeaders();
  std::string result = "HTTP/1.1 " + std::to_string(mStatus) + " ok\r\n\r\n" +
                       mHeaders.formatHeaders();
  boost::asio::async_write(socket, boost::asio::buffer(result), yield);
  mResponseBody->sendBody(socket, yield);
}

// void HttpResponse::setStreamResponse(StreamGenerator responseBody) {
//   mResponseBody = std::move(responseBody);
// }

// void HttpResponse::setTextBody(std::string responseBody) {
//   mHeaders.setContentLength(responseBody.length());
//   mHeaders.setContentType("text/plain");
//   mResponseBody = std::move(responseBody);
// }

} // namespace empty_d::http