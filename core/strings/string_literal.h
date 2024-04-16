//
// Created by chopk on 16.04.24.
//

#ifndef EMPTY_D_STRING_LITERAL_H
#define EMPTY_D_STRING_LITERAL_H

#include <cstddef>
#include <algorithm>

namespace core::utils {

    template<std::size_t N>
    class string_literal {
        constexpr explicit string_literal(const char (&str)[N]) {
            std::copy_n(str, N, value);
        }
        char value[N]{};
    };

}



#endif //EMPTY_D_STRING_LITERAL_H
