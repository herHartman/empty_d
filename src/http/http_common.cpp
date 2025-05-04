#include "http/http_common.hpp"

namespace empty_d::http::common {

std::ostream &operator<<(std::ostream &out, const HttpVersion &version) {
  return out << "HTTP/" << version.major << "." << version.minor;
}

} // namespace empty_d::http::common