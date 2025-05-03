#pragma once

namespace empty_d::http {
enum class HttpMethods {
  HTTP_OPTIONS = 0,
  HTTP_GET = 1,
  HTTP_HEAD = 2,
  HTTP_POST = 3,
  HTTP_PUT = 4,
  HTTP_PATCH = 5,
  HTTP_DELETE = 6,
  HTTP_TRACE = 7,
  HTTP_CONNECT = 8,
  COUNT = 9
};
}