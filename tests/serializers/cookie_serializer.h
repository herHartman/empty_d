//
// Created by chopk on 20.04.24.
//

#ifndef EMPTY_D_COOKIE_SERIALIZER_H
#define EMPTY_D_COOKIE_SERIALIZER_H

#include <boost/json.hpp>

using namespace boost;

class cookie;

class cookie_serializer {
public:
    [[nodiscard]] cookie deserialize(const json::value& json_value) const;
};


#endif //EMPTY_D_COOKIE_SERIALIZER_H
