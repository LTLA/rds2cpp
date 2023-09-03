#ifndef RDS2CPP_UTILS_PARSE_HPP
#define RDS2CPP_UTILS_PARSE_HPP

#include <algorithm>
#include <vector>
#include <cstdint>
#include <memory>

namespace rds2cpp {

inline std::runtime_error traceback(std::string base, const std::exception& e) {
    return std::runtime_error(base + "\n  - " + e.what());
}

inline std::runtime_error empty_error() {
    return std::runtime_error("no more bytes to read");
}

template<class Source_>
size_t get_length(Source_& src) {
    uint32_t initial = 0;
    try {
        for (int b = 0; b < 4; ++b) {
            if (!src.advance()) {
                throw empty_error();
            }
            initial <<= 8;
            initial += src.get();
        }
    } catch (std::exception& e) {
        throw traceback("failed to extract vector length", e);
    }

    if (initial != static_cast<uint32_t>(-1)) {
        return initial;
    }

    // Hack to deal with large lengths. 
    uint64_t full = 0;
    try {
        for (size_t b = 0; b < 8; ++b) {
            if (!src.advance()) {
                throw empty_error();
            }
            full <<= 8;
            full += src.get();
        }
    } catch (std::exception& e) {
        throw traceback("failed to extract large vector length", e);
    }

    return full;
}

inline bool little_endian() {
    const uint32_t value = 1;
    auto lsb = reinterpret_cast<const unsigned char *>(&value);
    return (*lsb == 1);
}

typedef std::array<unsigned char, 4> Header;

template<class Source_>
Header parse_header(Source_& src) try {
    Header details;
    int i = 0;
    for (int b = 0; b < 4; ++b, ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        details[i] = src.get();
    }
    return details;
} catch (std::exception& e) {
    throw traceback("failed to parse the R object header", e);
}

template<class Pointer, class Object>
void pointerize(Pointer& ptr, Object obj) {
    ptr.reset(new Object(std::move(obj)));
    return;
}

}

#endif
