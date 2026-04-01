#ifndef RDS2CPP_PARSE_ATOMIC_HPP
#define RDS2CPP_PARSE_ATOMIC_HPP

#include <cstddef>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "parse_single_string.hpp"

#include "sanisizer/sanisizer.hpp"

namespace rds2cpp {

namespace atomic_internal {

template<class Vector_, class Source_>
std::unique_ptr<Vector_> parse_integer_or_logical_body(Source_& src) {
    const auto len = get_length(src);
    auto output = std::make_unique<Vector_>(len);

    constexpr int width = 4;
    static_assert(width == sizeof(decltype(output->data[0])));
    const auto byte_length = sanisizer::product_unsafe<std::size_t>(width, len); // must be safe if we successfully allocated output.data.
    auto ptr = reinterpret_cast<unsigned char*>(output->data.data());
    quick_extract(src, byte_length, ptr);

    // Flipping endianness.
    if (little_endian()) {
        auto copy = ptr;
        for (I<decltype(len)> n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    return output;
}

}

template<class Source_>
std::unique_ptr<IntegerVector> parse_integer_body(Source_& src) try {
    return atomic_internal::parse_integer_or_logical_body<IntegerVector>(src);
} catch (std::exception& e) {
    throw traceback("failed to parse data for an integer vector", e);
    return std::unique_ptr<IntegerVector>();
}

template<class Source_>
std::unique_ptr<LogicalVector> parse_logical_body(Source_& src) try {
    return atomic_internal::parse_integer_or_logical_body<LogicalVector>(src);
} catch (std::exception& e) {
    throw traceback("failed to parse data for a logical vector", e);
    return std::unique_ptr<LogicalVector>();
}

template<class Source_>
std::unique_ptr<DoubleVector> parse_double_body(Source_& src) try {
    const auto len = get_length(src);
    auto output = std::make_unique<DoubleVector>(len);

    constexpr int width = 8;
    static_assert(width == sizeof(decltype(output->data[0])));
    const auto byte_length = sanisizer::product_unsafe<std::size_t>(width, len); // must be safe if we successfully allocated output->data.
    auto ptr = reinterpret_cast<unsigned char*>(output->data.data());
    quick_extract(src, byte_length, ptr);

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
    return std::unique_ptr<DoubleVector>();
}

template<class Source_>
std::unique_ptr<RawVector> parse_raw_body(Source_& src) try {
    const auto len = get_length(src);
    auto output = std::make_unique<RawVector>(len);

    auto ptr = reinterpret_cast<unsigned char*>(output->data.data());
    quick_extract(src, len, ptr);

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a raw vector", e);
    return std::unique_ptr<RawVector>();
}

template<class Source_>
std::unique_ptr<ComplexVector> parse_complex_body(Source_& src) try {
    const auto len = get_length(src);
    auto output = std::make_unique<ComplexVector>(len);

    constexpr int width = 16;
    static_assert(width == sizeof(decltype(output->data[0])));
    const auto byte_length = sanisizer::product_unsafe<std::size_t>(width, len); // must be safe if we successfully allocated output->data.
    auto ptr = reinterpret_cast<unsigned char*>(output->data.data());
    quick_extract(src, byte_length, ptr);

    // Flipping endianness for each double.
    if (little_endian()) {
        constexpr std::size_t single_width = width / 2;
        const auto single_length = sanisizer::product_unsafe<std::size_t>(len, 2); // must be safe, see logic above.
        auto copy = ptr;
        for (I<decltype(single_length)> n = 0; n < single_length; ++n, copy += single_width) {
            std::reverse(copy, copy + single_width);
        }
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a complex vector", e);
    return std::unique_ptr<ComplexVector>();
}

template<class Source_>
std::unique_ptr<StringVector> parse_string_body(Source_& src) try {
    const auto len = get_length(src);
    auto output = std::make_unique<StringVector>(len);
    for (I<decltype(len)> i = 0; i < len; ++i) {
        output->data[i] = parse_single_string(src);
    }
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse data for a string vector", e);
    return std::unique_ptr<StringVector>();
}

}

#endif
