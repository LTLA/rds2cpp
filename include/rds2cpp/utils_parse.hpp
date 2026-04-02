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
    if (len == 0) {
        return;
    }
    if (!src.valid()) {
        throw empty_error();
    }
    auto extracted = src.extract(len, output);
    if (extracted.first < len) {
        throw empty_error();
    }
}

inline bool little_endian() {
    const std::uint32_t value = 1;
    auto lsb = reinterpret_cast<const unsigned char *>(&value);
    return (*lsb == 1);
}

template<typename Integer_>
Integer_ quick_integer(byteme::BufferedReader<unsigned char>& src) {
    Integer_ val;
    auto ptr = reinterpret_cast<unsigned char*>(&val);
    quick_extract(src, sizeof(Integer_), ptr);
    if (little_endian()) {
        std::reverse(ptr, ptr + sizeof(Integer_));
    }
    return val;
}

template<class Source_>
std::size_t get_length(Source_& src) {
    std::int32_t initial = 0;
    try {
        initial = quick_integer<I<decltype(initial)> >(src);
    } catch (std::exception& e) {
        throw traceback("failed to extract vector length", e);
    }

    if (initial != -1) {
        if (initial < 0) {
            throw std::runtime_error("vector length should be non-negative");
        }
        return sanisizer::cast<std::size_t>(initial);
    }

    // Dealing with large lengths, represented as two unsigned integers according to Section 1.8 of R-internals.
    // We can't just load it into a single 64-bit integer due to endianness for each individual integer.
    std::uint32_t upper, lower;
    try {
        upper = quick_integer<I<decltype(upper)> >(src);
        lower = quick_integer<I<decltype(lower)> >(src);
    } catch (std::exception& e) {
        throw traceback("failed to extract large vector length", e);
    }

    std::uint64_t full = upper;
    full <<= 32;
    full += lower;
    return sanisizer::cast<std::size_t>(full);
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

}

#endif
