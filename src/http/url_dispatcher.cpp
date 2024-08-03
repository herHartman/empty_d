#include "url_dispatcher.hpp"
#include "http/protocol/http_methods.h"
#include <boost/algorithm/string/split.hpp>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

namespace empty_d::http {

void Resource::AddHandler(const HttpHandler &handler, HttpMethods method) {
  auto method_pos = static_cast<size_t>(method);
  if (handlers_[method_pos] == nullptr) {
    handlers_[method_pos] = handler;
  } else {
    throw std::runtime_error("handler with this method is exist");
  }
}

HttpHandler Resource::GetHandler(HttpMethods method) {
  return handlers_[static_cast<size_t>(method)];
}

std::vector<std::string> UrlDispatcher::SplitBySlash(const std::string &path) {
  std::vector<std::string> splitting_path;
  boost::split(splitting_path, path, [](char c) { return c == '/'; });
  return splitting_path;
}

bool UrlDispatcher::IsDynamicPathPart(const std::string &path_part) {
  if (path_part.size() > 2) {
    return path_part.starts_with('{') && path_part.ends_with('}');
  }
  return false;
}

void UrlDispatcher::AddHandler(const HttpHandler &handler, HttpMethods method,
                               const std::string &path) {
  if (std::optional<Resource> resource = routes_.lookup(path)) {
    resource->AddHandler(handler, method);
  } else {

    std::vector<std::string> splitting_path = UrlDispatcher::SplitBySlash(path);
    std::vector<PathArg> args;
    size_t current_path_start_pos = 1;
    for (const auto &path_segment : splitting_path) {
      if (UrlDispatcher::IsDynamicPathPart(path_segment)) {
        args.emplace_back(path_segment.substr(1, path_segment.size() - 1),
                          current_path_start_pos);
      }
      current_path_start_pos += path_segment.size();
    }
    Resource new_resource{path, std::move(args)};
    new_resource.AddHandler(handler, method);
  }
}

HttpHandler UrlDispatcher::GetHandler(const std::string &path,
                                      const HttpMethods method) {
  if (std::optional<Resource> resource = routes_.lookup(path)) {
    return resource->GetHandler(method);
  }
  return nullptr;
}

std::optional<Resource>
UrlDispatcher::GetResource(const std::string &path) const {
  return routes_.lookup(path);
}

} // namespace empty_d::http