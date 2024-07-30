#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace empty_d::http {
typedef std::string HeaderValue;
typedef std::string HeaderKey;
typedef std::unordered_map<HeaderKey, std::vector<HeaderValue>> HeaderValues;

class HttpHeaders {
public:
  using Iterator = HeaderValues::iterator;
  using ConstIterator = HeaderValues::const_iterator;

  using iterator = Iterator;
  using const_iterator = ConstIterator;

  class BasicHeaders {
    inline static constexpr char kAccept[] = "Accept";
    inline static constexpr char kAcceptCharset[] = "Accept-Charset";
    inline static constexpr char kAcceptEncoding[] = "Accept-Encoding";
    inline static constexpr char kacceptLanguage[] = "Accept-Language";
    inline static constexpr char kAcceptRanges[] = "Accept-Ranges";
    inline static constexpr char kAge[] = "Age";
    inline static constexpr char kAllow[] = "Allow";
    inline static constexpr char kAlternates[] = "Alternates";
    inline static constexpr char kAuthorization[] = "Authorization";
    inline static constexpr char kCache_control[] = "Cache-Control";
    inline static constexpr char kConnection[] = "Connection";
  };

  void Set(HeaderKey key, HeaderValue value);
  void Set(HeaderKey key, std::vector<HeaderValue> values);

  void Add(HeaderKey key, HeaderValue value);
  void Add(HeaderKey key, std::vector<HeaderValue> values);

  void SetBasicAuth(const std::string &username, const std::string &password);
  void SetBearerAuth(const std::string &token);
  void SetConnection(const std::string &connection);
  void SetContentLength(std::size_t content_size);
  void SetContentType(const std::string &media_type);
  void SetDate(const std::string &date);
  void SetHost(const std::string &host);
  void SetLocation(const std::string &location);
  void SetOrigin(const std::string &origin);

  std::string& GetContentLength();

  std::vector<HeaderValue> &GetHeaderValues(const HeaderKey &key);
  std::string FormatHeaders() const;

private:
  HeaderValues headers_{};
};

} // namespace empty_d::http
