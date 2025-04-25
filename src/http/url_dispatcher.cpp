#include "url_dispatcher.hpp"
#include "http/protocol/http_methods.h"
#include <boost/algorithm/string/split.hpp>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

namespace empty_d::http {

void Resource::addHandler(HttpHandler &handler, HttpMethods method) {
  auto method_pos = static_cast<size_t>(method);
  if (mHandlers[method_pos] == nullptr) {
    mHandlers[method_pos] = handler;
  } else {
    throw std::runtime_error("handler with this method is exist");
  }
}

HttpHandler Resource::getHandler(HttpMethods method) {
  return mHandlers[static_cast<size_t>(method)];
}

std::vector<std::string> UrlDispatcher::splitBySlash(const std::string &path) {
  std::vector<std::string> splitting_path;
  boost::split(splitting_path, path, [](char c) { return c == '/'; });
  return splitting_path;
}

bool UrlDispatcher::isDynamicPathPart(const std::string &path_part) {
  if (path_part.size() > 2) {
    return path_part.starts_with('{') && path_part.ends_with('}');
  }
  return false;
}

void UrlDispatcher::addHandler(HttpHandler handler, HttpMethods method,
                               std::string path) {
  if (boost::optional<Resource> resource = routes_.lookup(path)) {
    resource->addHandler(handler, method);
  } else {

    std::vector<std::string> splitting_path = UrlDispatcher::splitBySlash(path);
    std::vector<PathArg> args;
    size_t current_path_start_pos = 1;
    for (const auto &path_segment : splitting_path) {
      if (UrlDispatcher::isDynamicPathPart(path_segment)) {
        args.emplace_back(path_segment.substr(1, path_segment.size() - 1),
                          current_path_start_pos);
      }
      current_path_start_pos += path_segment.size();
    }
    Resource new_resource{path, std::move(args)};
    new_resource.addHandler(handler, method);
  }
}

HttpHandler UrlDispatcher::getHandler(const std::string &path,
                                      HttpMethods method) {
  if (boost::optional<Resource> resource = routes_.lookup(path)) {
    return resource->getHandler(method);
  }
  return nullptr;
}

boost::optional<Resource>
UrlDispatcher::getResource(const std::string &path) const {
  return routes_.lookup(path);
}

} // namespace empty_d::http