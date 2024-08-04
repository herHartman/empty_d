#pragma once

#include "http/protocol/request/http_request.h"
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <memory>
#include <vector>

namespace empty_d::http {

using boost::asio::awaitable;

class HttpRequestHandler {
public:
  awaitable<void> HandleRequest(request::HttpRequest &request);

private:
  
};
} // namespace empty_d::http