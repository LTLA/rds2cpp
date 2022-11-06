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

inline void inject_string(const char* ptr, size_t n, std::vector<unsigned char>& buffer) {
    auto p = reinterpret_cast<const unsigned char*>(ptr);
    buffer.insert(buffer.end(), p, p + n);
}

}

#endif
