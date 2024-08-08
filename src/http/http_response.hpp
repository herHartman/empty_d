#pragma once

#include "http/http_headers.h"
#include "http_status.h"

namespace empty_d::http {
class HttpResponse {
public:
  explicit HttpResponse(HttpStatus status) : status_(status){};

  std::string SerializeResponse();
  
  [[nodiscard]] HttpStatus GetStatusCode() const;
  void SetStatucCode(HttpStatus status);

  [[nodiscard]] HttpHeaders& GetHeaders();
  
  bool SetHeader(std::string name, std::string value);
  bool SetHeader(std::string_view name, std::string value);
  

private:
  HttpHeaders headers_;
  std::string body_;
  HttpStatus status_;
  std::string reason_;
};
} // namespace empty_d::http