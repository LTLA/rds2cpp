#ifndef RDS2CPP_PARSE_ATOMIC_HPP
#define RDS2CPP_PARSE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

namespace atomic_internal {

template<class Vector, class Source_>
Vector parse_integer_or_logical_body(Source_& src) {
    size_t len = get_length(src);
    Vector output(len);

    constexpr size_t width = 4;
    static_assert(width == sizeof(decltype(output.data[0])));
    size_t byte_length = width * len;

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    for (size_t i = 0; i < byte_length; ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        ptr[i] = src.get();
    }

    // Flipping endianness.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    return output;
}

}

template<class Source_>
IntegerVector parse_integer_body(Source_& src) try {
    return atomic_internal::parse_integer_or_logical_body<IntegerVector>(src);
} catch (std::exception& e) {
    throw traceback("failed to parse data for an integer vector", e);
}

template<class Source_>
LogicalVector parse_logical_body(Source_& src) try {
    return atomic_internal::parse_integer_or_logical_body<LogicalVector>(src);
} catch (std::exception& e) {
    throw traceback("failed to parse data for a logical vector", e);
}

template<class Source_>
DoubleVector parse_double_body(Source_& src) try {
    size_t len = get_length(src);
    DoubleVector output(len);

    constexpr size_t width = 8;
    static_assert(width == sizeof(decltype(output.data[0])));
    size_t byte_length = width * len;

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    for (size_t i = 0; i < byte_length; ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        ptr[i] = src.get();
    }

    // Flipping endianness.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a double vector", e);
}

template<class Source_>
RawVector parse_raw_body(Source_& src) try {
    size_t len = get_length(src);
    RawVector output(len);

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    for (size_t i = 0; i < len; ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        ptr[i] = src.get();
    } 

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a raw vector", e);
}

template<class Source_>
ComplexVector parse_complex_body(Source_& src) try {
    size_t len = get_length(src);
    ComplexVector output(len);

    constexpr size_t width = 16;
    static_assert(width == sizeof(decltype(output.data[0])));
    size_t byte_length = width * len;

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    for (size_t b = 0; b < byte_length; ++b) {
        if (!src.advance()) {
            throw empty_error();
        }
        ptr[b] = src.get();
    }

    // Flipping endianness for each double.
    if (little_endian()) {
        constexpr size_t single_width = width / 2;
        size_t single_length = len * 2;
        auto copy = ptr;
        for (size_t n = 0; n < single_length; ++n, copy += single_width) {
            std::reverse(copy, copy + single_width);
        }
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a complex vector", e);
}

template<class Source_>
StringVector parse_string_body(Source_& src) try {
    size_t len = get_length(src);
    StringVector output(len);
    for (size_t i = 0; i < len; ++i) {
        auto str = parse_single_string(src);
        output.data[i] = str.value;
        output.encodings[i] = str.encoding;
        output.missing[i] = str.missing;
    }
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a string vector", e);
}

}

#endif
