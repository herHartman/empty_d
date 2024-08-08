#pragma once

#include "definitions/strings/string_literal.h"
#include <type_traits>

template <typename class_owner, typename class_serializer>
class json_serializable {
public:
  template <typename json_field_type,
            empty_d::utils::string_literal serial_name, bool required>
  struct property_field {};

  typedef class_serializer serializer;
};

template <typename T>
concept is_serializable =
    std::is_base_of_v<json_serializable<T, typename T::serializer>, T>;
