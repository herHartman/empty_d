//
// Created by chopk on 20.04.24.
//

#ifndef EMPTY_D_COOKIE_H
#define EMPTY_D_COOKIE_H

#include <string>

#include "cookie_serializer.h"
#include "../../io/serializable/json_serializable.h"

struct cookie : json_serializable<cookie, cookie_serializer> {\
    explicit cookie(int id, std::string name, double price) : id(id), name(std::move(name)), price(price) {}

    int id;
    std::string name;
    double price;
};


#endif //EMPTY_D_COOKIE_H
