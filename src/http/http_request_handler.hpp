#pragma once

#include <boost/asio/awaitable.hpp>
#include <vector>
#include <memory>
#include <boost/asio.hpp>
#include "http/protocol/request/http_request.h"


namespace empty_d::http {

using boost::asio::awaitable;

class HttpRequestHandler {
public:
  awaitable<void> HandleRequest(request::HttpRequest& request);
private:
  
};
} // namespace empry_d::http