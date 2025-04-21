#pragma once

#include "http/http_response.hpp"
#include "http/protocol/http_methods.h"
#include "http/protocol/request/http_request.h"
#include "radix_tree/radix_tree_map.hpp"
#include <array>
#include <boost/function_types/detail/synthesize.hpp>
#include <functional>
#include <vector>
#include <boost/asio/spawn.hpp>
#include <boost/optional.hpp>

namespace empty_d { namespace http {

using HttpHandler = std::function<HttpResponse(empty_d::http::request::HttpRequest&, boost::asio::yield_context yield)>;

struct PathArg {
  std::string arg_name;
  size_t segment_pos;
};

template <typename T> 
struct type_from_member;

template <typename M, typename T>
struct type_from_member< M T::* > {
    using type = T;
};

template<typename Method, typename... Args>
auto make_handler(Method method, Args&&... ctor_args) {
  using T = typename type_from_member<Method>::type;
  return [method, ctor_args...](request::HttpRequest& request, boost::asio::yield_context yield) -> HttpResponse {
    T controller(ctor_args...);
    return (controller.*method)(request, yield);
  };
}

class Resource {
public:
  explicit Resource(std::string path_, std::vector<PathArg> expected_args_)
      : path_(std::move(path_)), expected_args_{std::move(expected_args_)} {}

  void AddHandler(HttpHandler &handler, HttpMethods method);
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
  HttpHandler GetHandler(const std::string &path, HttpMethods method);

  boost::optional<Resource> GetResource(const std::string &path) const;
  
private:
  radix_tree::radix_tree_map<Resource, char> routes_{};

  static std::vector<std::string> SplitBySlash(const std::string &path);
  static bool IsDynamicPathPart(const std::string &path_part);
};
} } // namespace empty_d::http