#include "http_response.hpp"

namespace empty_d { namespace http {

  void HttpWriter::enableChunked() {
    mChunked = true;
  }

  void HttpWriter::enableCompression() {
    mCompression = true;
  }

  void HttpWriter::setEof() {
    mEof = true;
  }

  void HttpWriter::write(const std::string& buffer) {
    if (mChunked) {

    }
  }
  
  std::string HttpResponse::serializeResponse() {
    
  }
} }