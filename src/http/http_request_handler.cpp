#include "http_request_handler.hpp"

namespace empty_d::http {
awaitable<void>
HttpRequestHandlerBase::HandleRequest(request::HttpRequest &request) {}
} // namespace empty_d::http