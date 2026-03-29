#ifndef RDS2CPP_UTILS_PARSE_HPP
#define RDS2CPP_UTILS_PARSE_HPP

#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <stdexcept>

#include "utils_other.hpp"

#include "byteme/byteme.hpp"

namespace rds2cpp {

inline std::runtime_error traceback(std::string base, const std::exception& e) {
    return std::runtime_error(base + "\n  - " + e.what());
}

inline std::runtime_error empty_error() {
    return std::runtime_error("no more bytes to read");
}

inline void quick_extract(byteme::BufferedReader<unsigned char>& src, std::size_t len, unsigned char* output) {
    if (!src.advance()) {
        throw empty_error();
    }
    auto extracted = src.extract_until(len, output);
    if (extracted != len) {
        throw empty_error();
    }
}

inline char as_char(unsigned char val) {
    return *reinterpret_cast<const char*>(&val); // make sure we interpret this as a char.
}

template<class Source_>
std::size_t get_length(Source_& src) {
    std::uint32_t initial = 0;
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

    if (initial != static_cast<std::uint32_t>(-1)) {
        return sanisizer::cast<std::size_t>(initial);
    }

    // Hack to deal with large lengths. 
    std::uint64_t full = 0;
    try {
        for (int b = 0; b < 8; ++b) {
            if (!src.advance()) {
                throw empty_error();
            }
            full <<= 8;
            full += src.get();
        }
    } catch (std::exception& e) {
        throw traceback("failed to extract large vector length", e);
    }

    return sanisizer::cast<std::size_t>(full);
}

inline bool little_endian() {
    const std::uint32_t value = 1;
    auto lsb = reinterpret_cast<const unsigned char *>(&value);
    return (*lsb == 1);
}

typedef std::array<unsigned char, 4> Header;

template<class Source_>
Header parse_header(Source_& src) try {
    Header details;
    quick_extract(src, details.size(), details.data());
    return details;
} catch (std::exception& e) {
    throw traceback("failed to parse the R object header", e);
    return Header();
}

template<class Pointer, class Object>
void pointerize(Pointer& ptr, Object obj) {
    ptr.reset(new Object(std::move(obj)));
    return;
}

}

#endif
