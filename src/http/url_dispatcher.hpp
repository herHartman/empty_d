#pragma once

#include "http/http_response.hpp"
#include "http/protocol/http_methods.h"
#include "http/protocol/request/http_request.h"
#include "radix_tree/radix_tree_map.hpp"
#include <array>
#include <functional>
#include <vector>

namespace empty_d::http {

using HttpHandler = std::function<HttpResponse(request::HttpRequest)>;

struct PathArg {
  std::string arg_name;
  size_t segment_pos;
};

class Resource {
public:
  explicit Resource(std::string path_, std::vector<PathArg> expected_args_)
      : path_(std::move(path_)), expected_args_{std::move(expected_args_)} {}

  void AddHandler(const HttpHandler &handler, HttpMethods method);
  HttpHandler GetHandler(HttpMethods method);

  [[nodiscard]] const std::vector<PathArg> &GetPathArgs() const {
    return expected_args_;
  }
  
private:
  std::string path_;
  std::array<HttpHandler, static_cast<size_t>(HttpMethods::COUNT)> handlers_{
      nullptr};
  std::vector<PathArg> expected_args_;
};

class UrlDispatcher {
public:
  struct MatchInfo {
    HttpHandler handler;
    std::string route_;
  };

  using Routes = radix_tree::radix_tree_map<Resource, char>;

  void AddHandler(const HttpHandler &handler, HttpMethods method,
                  const std::string &path);
  HttpHandler GetHandler(const std::string &path, const HttpMethods method);

  [[nodiscard]] std::optional<Resource>
  GetResource(const std::string &path) const;

private:
  Routes routes_{};

  static std::vector<std::string> SplitBySlash(const std::string &path);
  static bool IsDynamicPathPart(const std::string &path_part);
};
} // namespace empty_d::http