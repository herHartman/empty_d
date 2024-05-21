#include "http_request_parser.h"
#include "http/protocol/http_methods.h"
#include "http/protocol/request/http_request.h"
#include <cctype>
#include <optional>
#include <string_view>
#include <tuple>

namespace http {

/*
 *   token          = 1*tchar
 *
 *   tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
 *                  / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
 *                   / DIGIT / ALPHA
 *
 *                    ; any VCHAR, except delimiters
 */
static constexpr char tokens[256] = {
    /*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel */
    0, 0, 0, 0, 0, 0, 0, 0,
    /*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si */
    0, 0, 0, 0, 0, 0, 0, 0,
    /*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
    0, 0, 0, 0, 0, 0, 0, 0,
    /*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
    0, 0, 0, 0, 0, 0, 0, 0,
    /*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
    ' ', '!', 0, '#', '$', '%', '&', '\'',
    /*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
    0, 0, '*', '+', 0, '-', '.', 0,
    /*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
    '0', '1', '2', '3', '4', '5', '6', '7',
    /*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
    '8', '9', 0, 0, 0, 0, 0, 0,
    /*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
    0, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    /*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    /*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    /*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
    'x', 'y', 'z', 0, 0, 0, '^', '_',
    /*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    /* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    /* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    /* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
    'x', 'y', 'z', 0, '|', 0, '~', 0};

enum class url_state {
  URL_START = 0,
  URL_SCHEMA,
  URL_PATH,
  URL_SCHEMA_SLASH,
  URL_SCHEMA_SLASH_SLASH,
  URL_SERVER,
  QUERY_STRING_START,
  QUERY_PARAM_NAME,
  QUERY_PARAM_VALUE
};

static std::unordered_map<std::string_view, http_methods> http_methods_map = {
    {"OPTIONS", http_methods::HTTP_OPTIONS},
    {"GET", http_methods::HTTP_GET},
    {"HEAD", http_methods::HTTP_HEAD},
    {"POST", http_methods::HTTP_POST},
    {"PUT", http::HTTP_PUT},
    {"PATCH", http_methods::HTTP_PATCH},
    {"DELETE", http_methods::HTTP_DELETE},
    {"TRACE", http_methods::HTTP_TRACE},
    {"CONNECT", http_methods::HTTP_CONNECT}};

inline bool is_header_char(char ch) {
  return (ch == CR || ch == LF || ch == 9 ||
          ((unsigned char)ch > 31 && ch != 127));
}

headers_parser::parse_result
headers_parser::parse_headers(const char *data, std::size_t len,
                              std::size_t *body_start_index) {
  parse_header_state current_state =
      parse_header_state::PARSE_HEADER_FIELD_START;
  std::size_t current_substr_pos = 0;
  std::size_t i = 0;

  while (i < len) {
    switch (current_state) {
    case parse_header_state::PARSE_HEADER_FIELD_START:
      if (data[i] == LF) {
        return {std::move(result_), 0};
      }

      current_state_ = parse_header_state::PARSE_NAME;

    case parse_header_state::PARSE_NAME:
      for (; i < len; ++i) {
        char token = tokens[data[i]];
        if (!token) {
          break;
        }
      }

      if (data[i] == HEADER_SEPARATOR) {
        current_header_name_ =
            std::string_view(&data[current_substr_pos], i - current_substr_pos);
        current_state_ = parse_header_state::PARSE_SPACES_BEFORE_VALUE;
        current_substr_pos = i + 1;
        break;
      } else {
        return {{}, -1};
      }
    case parse_header_state::PARSE_SPACES_BEFORE_VALUE:
      if (isspace(data[i])) {
        continue;
      } else if (tokens[data[i]]) {
        current_state_ = parse_header_state::PARSE_VALUE;
        break;
      } else {
        return {{}, -1};
      }
    case parse_header_state::PARSE_VALUE:
      for (; i < len; ++i) {
        if (data[i] == CR) {
          if (i < len - 1 && data[i + 1] == LF) {
            result_.add(current_header_name_,
                        std::string_view(&data[current_substr_pos], i));
            i += 2;
            current_substr_pos = i;
            current_state_ = parse_header_state::PARSE_HEADER_FIELD_START;
            break;
          } else {
            return {{}, -1};
          }
        }

        if (!is_header_char(data[i])) {
          return {{}, -1};
        }
      }
    }
    ++i;
  }
  return {{}, -1};
}

request http_request_parser::parse_message(const char *data, std::size_t len,
                                           std::size_t *body_start_position) {
  std::size_t current_substr_start_pos = 0;
  for (int i = 0; i < len; ++i) {
    switch (current_state_) {
    case request_state::PARSE_METHOD:
      if (isspace(data[i])) {
        current_state_ = request_state::PARSE_PATH;
        http_methods method = http_methods_map[std::string_view(
            &data[current_substr_start_pos], i - current_substr_start_pos)];
        current_substr_start_pos = i + 1;
      }
      break;
    case request_state::PARSE_PATH:
      if (isspace(data[i])) {
        request_builder_.append_path(std::string_view(
            &data[current_substr_start_pos], i - current_substr_start_pos));

        current_state_ = request_state::PARSE_HTTP_VERSION;
        current_substr_start_pos = i + 1;
      }
      break;
    case request_state::PARSE_HTTP_VERSION:
      if (data[i] == CR) {
        request_builder_.append_http_version(std::string_view(
            &data[current_substr_start_pos], i - current_substr_start_pos));

        current_state_ = request_state::PARSE_HEADERS;
      }
      break;
    case request_state::PARSE_HEADERS:
      if (data[i] == LF)
        continue;
      auto [headers, last_position] =
          headers_parser_.parse_headers(data, len, body_start_position);
      request_builder_.append_headers(std::move(headers.value()));
      return std::move(request_builder_.build_request());
    }
  }
  return std::move(request_builder_.build_request());
}
}; // namespace http
