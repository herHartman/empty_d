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

  struct BasicHeaders {
    static constexpr char kAccept[] = "Accept";
    static constexpr char kAcceptCharset[] = "Accept-Charset";
    static constexpr char kAcceptEncoding[] = "Accept-Encoding";
    static constexpr char kacceptLanguage[] = "Accept-Language";
    static constexpr char kAcceptRanges[] = "Accept-Ranges";
    static constexpr char kAge[] = "Age";
    static constexpr char kAllow[] = "Allow";
    static constexpr char kAlternates[] = "Alternates";
    static constexpr char kAuthorization[] = "Authorization";
    static constexpr char kCache_control[] = "Cache-Control";
    static constexpr char kConnection[] = "Connection";
    static constexpr char kContentLength[] = "Content-Length";
    static constexpr char kHost[] = "Host";
  };

  void Set(const HeaderKey &key, HeaderValue value);
  void Set(const HeaderKey &key, std::vector<HeaderValue> values);

  void Add(const HeaderKey &key, HeaderValue value);
  void Add(const HeaderKey &key, std::vector<HeaderValue> values);

  void SetBasicAuth(const std::string &username, const std::string &password);
  void SetBearerAuth(const std::string &token);
  void SetConnection(const std::string &connection);
  void SetContentLength(std::size_t content_size);
  void SetContentType(const std::string &media_type);
  void SetDate(const std::string &date);
  void SetHost(const std::string &host);
  void SetLocation(const std::string &location);
  void SetOrigin(const std::string &origin);

  size_t GetContentLength();
  std::string &GetHost();

  std::vector<HeaderValue> &GetHeaderValues(const HeaderKey &key);
  std::string FormatHeaders() const;

private:
  HeaderValues headers_{};
};

} // namespace empty_d::http
