#pragma once

#include "http/http_response.hpp"
#include "http/protocol/http_methods.h"
#include "http/protocol/request/http_request.h"
#include "radix_tree/radix_tree_map.hpp"
#include <array>
#include <boost/asio/spawn.hpp>
#include <boost/function_types/detail/synthesize.hpp>
#include <boost/optional.hpp>
#include <boost/variant/variant.hpp>
#include <functional>
#include <vector>

namespace empty_d::http {

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>, request::HttpRequest&, boost::asio::yield_context yield)>;

struct PathArg {
  std::string arg_name;
  size_t segment_pos;
};

class Resource {
public:
  explicit Resource(std::string path, std::vector<PathArg> expectedArgs)
      : mPath(std::move(path)), mExpectedArgs{std::move(expectedArgs)} {}

  void addHandler(HttpHandler &handler, HttpMethods method);
  HttpHandler getHandler(HttpMethods method);

  [[nodiscard]] const std::vector<PathArg> &getPathArgs() const {
    return mExpectedArgs;
  }

private:
  std::string mPath;
  std::array<HttpHandler, static_cast<size_t>(HttpMethods::COUNT)> mHandlers{};
  std::vector<PathArg> mExpectedArgs;
};

class UrlDispatcher {
public:
  struct MatchInfo {
    HttpHandler handler;
    std::string route;
  };

  using Routes = radix_tree::radix_tree_map<Resource, char>;

  void addHandler(HttpHandler handler, HttpMethods method, std::string path);

  HttpHandler getHandler(const std::string &path, HttpMethods method);

  [[nodiscard]] boost::optional<Resource>
  getResource(const std::string &path) const;

private:
  radix_tree::radix_tree_map<Resource, char> routes_{};

  static std::vector<std::string> splitBySlash(const std::string &path);
  static bool isDynamicPathPart(const std::string &path_part);
};
} // namespace empty_d::http