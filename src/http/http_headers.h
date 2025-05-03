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

  Iterator begin() noexcept;
  Iterator end() noexcept;

  ConstIterator begin() const noexcept;
  ConstIterator end() const noexcept;

  ConstIterator cbegin() const noexcept;
  ConstIterator cend() const noexcept;

  Iterator find(const HeaderKey &key);
  ConstIterator find(const HeaderKey &key) const;

  HeaderValues &headers() noexcept;
  const HeaderValues &headers() const noexcept;

  bool empty() const noexcept;
  size_t size() const noexcept;

  Iterator erase(ConstIterator pos);
  size_t erase(const HeaderKey &key);
  void clear() noexcept;

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
    static constexpr char kSetCookie[] = "Set-Cookie";
    static constexpr char kWWWAuthenticate[] = "WWW-Authenticate";
    static constexpr char kProxyAuthenticate[] = "Proxy-Authenticate";
    static constexpr char kLink[] = "Link";
    static constexpr char kVary[] = "Vary";
    static constexpr char kStrictTransportSecurity[] =
        "Strict-Transport-Security";
  };

  void set(const HeaderKey &key, HeaderValue value);
  void set(const HeaderKey &key, std::vector<HeaderValue> values);

  void add(const HeaderKey &key, HeaderValue value);
  void add(const HeaderKey &key, std::vector<HeaderValue> values);

  void setBasicAuth(std::string username, std::string password);
  void setBearerAuth(std::string token);
  void setConnection(std::string connection);
  void setContentLength(std::size_t contentSize);
  void setContentType(std::string mediaType);
  void setDate(std::string date);
  void setHost(std::string host);
  void setLocation(std::string location);
  void setOrigin(std::string origin);

  size_t getContentLength() const;
  const std::string &getHost() const;

  std::vector<HeaderValue> &getHeaderValues(const HeaderKey &key);
  std::string formatHeaders() const;

  const std::string &getContentType() const;

  void addHeaders(HttpHeaders headers);

private:
  static bool canCominedHeaders(const std::string &headerKey);

  HeaderValues mHeaders{};
};

} // namespace empty_d::http
