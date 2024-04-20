//
// Created by chopk on 20.04.24.
//

#include "cookie_serializer.h"
#include "cookie.h"
#include <boost/json.hpp>

cookie cookie_serializer::deserialize(const json::value& json_value) const {
    auto const& obj = json_value.get_object();
    if (!obj.empty()) {
        int id = static_cast<int>(obj.find("id")->value().as_int64());
        auto const name = obj.find("name")->value().as_string();
        auto const price = obj.find("price")->value().as_double();
        return cookie{id, name.c_str(), price};
    }
    return {};
}