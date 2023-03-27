#ifndef RDS2CPP_UTILS_WRITE_HPP
#define RDS2CPP_UTILS_WRITE_HPP

#include <algorithm>
#include <vector>
#include <cstdint>
#include "utils_parse.hpp"

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

    auto ptr = reinterpret_cast<unsigned char*>(&big);
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

template<class Object, typename = int>
struct has_attributes_for_writing {
    static constexpr bool value = false;
};

template<class Object>
struct has_attributes_for_writing<Object, decltype((void) std::declval<Object>().attributes, 0)> {
    static constexpr bool value = true;
};

template<class Attributes>
inline unsigned char inject_attribute_header(Attributes& attributes) {
    unsigned char bit = 0;

    if (!attributes.names.empty()) {
        bit |= 0x2;
        for (const auto& x : attributes.names) {
            if (x == "class") {
                bit |= 0x1;
                break;
            }
        }
    }

    return bit;
}

template<class Object>
void inject_header(Object& vec, std::vector<unsigned char>& buffer) {
    buffer.insert(buffer.end(), 2, 0);

    if constexpr(has_attributes_for_writing<Object>::value) {
        buffer.push_back(inject_attribute_header(vec.attributes));
    } else {
        buffer.push_back(0);
    }

    // cast from enum should be safe, as SEXPTypes are also unsigned chars.
    buffer.push_back(static_cast<unsigned char>(vec.type())); 
    return;
}

inline void inject_header(SEXPType type, std::vector<unsigned char>& buffer) {
    buffer.insert(buffer.end(), 3, 0);
    buffer.push_back(static_cast<unsigned char>(type));
    return;
}

template<class Attributes>
void inject_header(SEXPType type, Attributes& attributes, std::vector<unsigned char>& buffer) {
    buffer.insert(buffer.end(), 2, 0);
    buffer.push_back(inject_attribute_header(attributes));
    buffer.push_back(static_cast<unsigned char>(type));
    return;
}

inline void inject_next_pairlist_header(bool tagged, std::vector<unsigned char>& buffer) {
    buffer.insert(buffer.end(), 2, 0);
    buffer.push_back(tagged ? 0x4 : 0); // has tag.
    buffer.push_back(static_cast<unsigned char>(SEXPType::LIST));
    return;
}

}

#endif
