#ifndef RDS2CPP_UTILS_WRITE_HPP
#define RDS2CPP_UTILS_WRITE_HPP

#include <algorithm>
#include <vector>
#include <cstdint>
#include "utils.hpp"

namespace rds2cpp {

inline void inject_integer(int32_t value, std::vector<unsigned char>& buffer) {
    auto ptr = reinterpret_cast<unsigned char*>(&value);
    constexpr size_t width = 4;

    if (little_endian()) {
        std::reverse(ptr, ptr + width);
    }
    buffer.insert(buffer.end(), ptr, ptr + width);
}

inline void inject_length(size_t value, std::vector<unsigned char>& buffer) {
    if (value <= 2147483647) {
        inject_integer(value, buffer);
        return;
    }

    inject_integer(-1, buffer);
    uint64_t big = value;

    auto ptr = reinterpret_cast<unsigned char*>(&value);
    constexpr size_t width = 8;
    if (little_endian()) {
        std::reverse(ptr, ptr + width/2);
        std::reverse(ptr + width/2, ptr + width);
    }

    buffer.insert(buffer.end(), ptr, ptr + width);
}

inline void inject_string(const char* ptr, size_t n, std::vector<unsigned char>& buffer) {
    auto p = reinterpret_cast<const unsigned char*>(ptr);
    buffer.insert(buffer.end(), p, p + n);
}

template<bool has_attributes, class Object>
void inject_header(Object& vec, std::vector<unsigned char>& buffer) {
    buffer.push_back(0);
    buffer.push_back(0);

    if constexpr(has_attributes) {
        buffer.push_back(vec.attributes.names.empty() ? 0 : 2);
    } else {
        buffer.push_back(0);
    }

    // cast from enum should be safe, as SEXPTypes are also unsigned chars.
    buffer.push_back(static_cast<unsigned char>(vec.type())); 
}


}

#endif
