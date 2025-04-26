#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace empty_d::http {
class MimeType {
public:
  static constexpr char kSeparator = '/';
  static constexpr char kParamKeyValueSeparator = '=';
  static constexpr char kParamSeparator = ';';

  static constexpr char kAllValue[] = "*/*";

  static constexpr char kApplicationTypeValue[] = "application";
  static constexpr char kImageTypeValue[] = "image";
  static constexpr char kVideoTypeValue[] = "video";
  static constexpr char kMultipartTypeValue[] = "multipart";
  static constexpr char kTextTypeValue[] = "text";

  static constexpr char kAtomXMLValue[] = "atom+xml";
  static constexpr char kCBORValue[] = "cbor";
  static constexpr char kFormURLEncodedValue[] = "x-www-form-urlencoded";
  static constexpr char kGraphQLResponseValue[] = "graphql-response+json";
  static constexpr char kJsonValue[] = "json";
  static constexpr char kOctetStreamValue[] = "octet-stream";
  static constexpr char kPDFValue[] = "pdf";
  static constexpr char kProblemJSONValue[] = "problem+json";
  static constexpr char kProblemXMLValue[] = "problem+xml";
  static constexpr char kProtobufValue[] = "x-protobuf";
  static constexpr char kRSSXMLValue[] = "rss+xml";
  static constexpr char kNDJSONValue[] = "x-ndjson";
  static constexpr char kStreamJSONValue[] = "stream+json";
  static constexpr char kXHTMLXMLValue[] = "xhtml+xml";
  static constexpr char kXMLValue[] = "xml";
  static constexpr char kYAMLValue[] = "yaml";
  static constexpr char kGIFValue[] = "gif";
  static constexpr char kJPEGValue[] = "jpeg";
  static constexpr char kPNGValue[] = "png";
  static constexpr char kFormDataValue[] = "form-data";
  static constexpr char kFormMixedValue[] = "mixed";
  static constexpr char kRelatedValue[] = "related";
  static constexpr char kEventStreamValue[] = "event-stream";
  static constexpr char kHTMLValue[] = "html";
  static constexpr char kMarkdownValue[] = "markdown";
  static constexpr char kPlainValue[] = "plain";

  MimeType(std::string type, std::string subtype)
      : type_(std::move(type)), subtype_(std::move(subtype)) {}

private:
  std::string type_;
  std::string subtype_;
  std::optional<std::string> charset_;
  std::unordered_map<std::string, std::string> parameters_;
};

static inline const MimeType kApplicationCBOR{MimeType::kApplicationTypeValue,
                                              MimeType::kCBORValue};

static inline const MimeType kApplicationFormURLEncoded{
    MimeType::kApplicationTypeValue, MimeType::kFormURLEncodedValue};

static inline const MimeType kApplicationGraphQLResponse{
    MimeType::kApplicationTypeValue, MimeType::kGraphQLResponseValue};

static inline const MimeType kApplicationJson{MimeType::kApplicationTypeValue,
                                              MimeType::kJsonValue};

static inline const MimeType kApplicationOctetStream{
    MimeType::kApplicationTypeValue, MimeType::kOctetStreamValue};

static inline const MimeType kApplicationPDF{MimeType::kApplicationTypeValue,
                                             MimeType::kPDFValue};

static inline const MimeType kApplicationProblemJSON{
    MimeType::kApplicationTypeValue, MimeType::kProblemJSONValue};

static inline const MimeType kApplicationProblemXML{
    MimeType::kApplicationTypeValue, MimeType::kProblemXMLValue};

static inline const MimeType kApplicationProtobuf{
    MimeType::kApplicationTypeValue, MimeType::kProtobufValue};

static inline const MimeType kApplicationRSSXML{MimeType::kApplicationTypeValue,
                                                MimeType::kRSSXMLValue};

static inline const MimeType kApplcationNDJSON{MimeType::kApplicationTypeValue,
                                               MimeType::kNDJSONValue};

static inline const MimeType kApplicationStreamJSON{
    MimeType::kApplicationTypeValue, MimeType::kStreamJSONValue};

static inline const MimeType kApplicationXHTMLXML{
    MimeType::kApplicationTypeValue, MimeType::kXHTMLXMLValue};

static inline const MimeType kApplicationXML{MimeType::kApplicationTypeValue,
                                             MimeType::kXMLValue};

static inline const MimeType kApplicationYAML{MimeType::kApplicationTypeValue,
                                              MimeType::kYAMLValue};

static inline const MimeType kImageGIF{MimeType::kImageTypeValue,
                                       MimeType::kGIFValue};

static inline const MimeType kImageJPEG{MimeType::kImageTypeValue,
                                        MimeType::kJPEGValue};

static inline const MimeType kImagePNG{MimeType::kImageTypeValue,
                                       MimeType::kPNGValue};

static inline const MimeType kMultipartFormData{MimeType::kMultipartTypeValue,
                                                MimeType::kFormDataValue};

static inline const MimeType kMultipartFormMixed{MimeType::kMultipartTypeValue,
                                                 MimeType::kFormMixedValue};

static inline const MimeType kMultipartRelated{MimeType::kMultipartTypeValue,
                                               MimeType::kRelatedValue};

static inline const MimeType kTextEventStream{MimeType::kTextTypeValue,
                                              MimeType::kEventStreamValue};

static inline const MimeType kTextHTML{MimeType::kTextTypeValue,
                                       MimeType::kHTMLValue};

static inline const MimeType kTextMarkdown{MimeType::kTextTypeValue,
                                           MimeType::kMarkdownValue};

static inline const MimeType kTextPlain{MimeType::kTextTypeValue,
                                        MimeType::kPlainValue};

static inline const MimeType kTextXML{MimeType::kTextTypeValue,
                                      MimeType::kXMLValue};

} // namespace empty_d::http