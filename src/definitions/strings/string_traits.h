#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <climits>
#include <algorithm>

namespace empty_d::helpers {

template<typename T>
struct StringTraits {
    static_assert(std::is_same<T, void>::value, "No specialization exists for this type. Add custom specialization.");
};

namespace detail {

    template<typename T, bool Signed = std::is_signed<T>::value>
    struct IntegerConverter;

    template<typename T>
    struct IntegerConverter<T, true> {
        static T convert(const std::string& str, const char* typeName) {
            char* end;
            long long val = std::strtoll(str.c_str(), &end, 0);
            if (errno == ERANGE || val < std::numeric_limits<T>::min() || std::numeric_limits<T>::max()) {
                throw std::out_of_range("Value out of range for " + std::string(typeName));
            }

            if (end != str.c_str() + str.size()) {
                throw std::invalid_argument("Invalid characters in " + std::string(typeName));
            }
            return static_cast<T>(val);
        }
    };

    template<typename T>
    struct IntegerConverter<T, false> {
        static T convert(const std::string& str, const char* typeName) {
            char* end;
            errno = 0;
            unsigned long long val = std::strtoull(str.c_str(), &end, 0);
            
            if (errno == ERANGE || val > std::numeric_limits<T>::max()) {
                throw std::out_of_range("Value out of range for " + std::string(typeName));
            }
            if (end != str.c_str() + str.size()) {
                throw std::invalid_argument("Invalid characters in " + std::string(typeName));
            }
            return static_cast<T>(val);
        }
    };
    
    template<typename T>
    T string_to_float(const std::string& str) {
        char* end;
        errno = 0;
        double val = std::strtod(str.c_str(), &end);
        if (errno == ERANGE) throw std::out_of_range("Value out of range");
        if (end != str.c_str() + str.size()) throw std::invalid_argument("Invalid characters");
        return static_cast<T>(val);
    }
}

#define INTEGER_SPECIALIZATION(type, name) \
template <> \
struct StringTraits<type> { \
    static type convert(const std::string& str) { \
        try { \
            return detail::IntegerConverter<type>::convert(str, name); \
        } catch (const std::exception& e) { \
            throw std::runtime_error(std::string("Failed to convert to " name ": ") + e.what()); \
        } \
    } \
};

INTEGER_SPECIALIZATION(int, "int")
INTEGER_SPECIALIZATION(unsigned int, "unsigned int")
INTEGER_SPECIALIZATION(long, "long")
INTEGER_SPECIALIZATION(unsigned long, "unsigned long")
INTEGER_SPECIALIZATION(long long, "long long")
INTEGER_SPECIALIZATION(unsigned long long, "unsigned long long")

template<>
struct StringTraits<float> {
    static float convert(const std::string& str) {
        try {
            return detail::string_to_float<float>(str);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to convert to float: ") + e.what());
        }
    }
};

template<>
struct StringTraits<double> {
    static float convert(const std::string& str) {
        try {
            return detail::string_to_float<double>(str);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to convert to double: ") + e.what());
        }
    }
};

template <>
struct StringTraits<bool> {
    static bool convert(const std::string& str) {
        std::string s = str;
        std::transform(s.begin(), s.end(), s.begin(), [](char c) { 
            return static_cast<char>(std::tolower(c)); 
        });
        
        if (s == "true" || s == "1") return true;
        if (s == "false" || s == "0") return false;
        
        try {
            return StringTraits<int>::convert(str) != 0;
        } catch (...) {
            throw std::runtime_error("Invalid boolean value: " + str);
        }
    }
};


// Специализации для строковых типов
template <>
struct StringTraits<std::string> {
    static std::string convert(const std::string& str) { return str; }
};


}