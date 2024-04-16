//
// Created by chopk on 16.04.24.
//

#ifndef EMPTY_D_JSON_SERIALIZABLE_H
#define EMPTY_D_JSON_SERIALIZABLE_H

#include "../../core/strings/string_literal.h"

template <typename class_owner>
class json_serializable {
    template<typename json_field_type, core::utils::string_literal serial_name, bool required>
    struct property_field {};

};


#endif //EMPTY_D_JSON_SERIALIZABLE_H
