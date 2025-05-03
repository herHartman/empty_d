#pragma once

#include <string>

namespace empty_d::strings {
    template<class Char>
    struct ichar_traits : public std::char_traits<Char> {
        static bool eq(Char c1, Char c2) {
            return std::toupper(c1) == std::toupper(c2);
        }

        static bool lt(Char c1, Char c2) {
            return std::toupper(c1) < std::toupper(c2);
        }

        static int compare(const Char* s1, const Char* s2, size_t n) {
            for (; n-- != 0; ++s1, ++s2) {
                if (std::toupper(*s1) < std::toupper(*s2)) return -1;
                else if (std::toupper(*s1) > std::toupper(*s2)) return 1;
            }
            return 0;
        }

        static const char* find(const Char* s, int n, Char a) {
            for (a = std::toupper(a); n-- != 0; ++s) {
                if (std::toupper(*s) == a) return s;
            }
            return nullptr;
        }
    };

    template<class Char>
    using ibasic_string = std::basic_string<Char, ichar_traits<Char>>;
    using istring = ibasic_string<char>;
    using iwstring = ibasic_string<wchar_t>;

    template<class Char, class Traits, class Char2>
    std::basic_ostream<Char, Traits>& operator<<(
        std::basic_ostream<Char, Traits>& os,
        const ibasic_string<Char2>& str
    ) {
        return os.write(os.data(), str.size());
    }
}