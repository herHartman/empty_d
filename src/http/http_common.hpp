#pragma once

#include <cstdint>
#include <ostream>
namespace empty_d::http::common {

struct HttpVersion {
  uint32_t major;
  uint32_t minor;
};

std::ostream &operator<<(std::ostream &out, const HttpVersion &version);

} // namespace empty_d::http::common