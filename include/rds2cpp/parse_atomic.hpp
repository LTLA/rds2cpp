#ifndef RDS2CPP_PARSE_ATOMIC_HPP
#define RDS2CPP_PARSE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

namespace atomic_internal {

template<class Vector, class Reader>
Vector parse_integer_or_logical_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    Vector output(len);

    constexpr size_t width = 4;
    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    bool ok = extract_up_to(reader, leftovers, width * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );
    if (!ok) {
        throw std::runtime_error("failed to parse data for an integer/logical vector");
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

template<class Reader>
IntegerVector parse_integer_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    return atomic_internal::parse_integer_or_logical_body<IntegerVector>(reader, leftovers);
}

template<class Reader>
LogicalVector parse_logical_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    return atomic_internal::parse_integer_or_logical_body<LogicalVector>(reader, leftovers);
}

template<class Reader>
DoubleVector parse_double_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    DoubleVector output(len);

    constexpr size_t width = 8;
    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    bool ok = extract_up_to(reader, leftovers, width * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );
    if (!ok) {
        throw std::runtime_error("failed to parse data for a double vector");
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

template<class Reader>
RawVector parse_raw_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    RawVector output(len);

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    bool ok = extract_up_to(reader, leftovers, len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );
    if (!ok) {
        throw std::runtime_error("failed to parse data for a raw vector");
    }

    return output;
}

template<class Reader>
ComplexVector parse_complex_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    ComplexVector output(len);

    constexpr size_t width = 16;
    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    bool ok = extract_up_to(reader, leftovers, width * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );
    if (!ok) {
        throw std::runtime_error("failed to parse data for a complex vector");
    }

    // Flipping endianness for each double.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len * 2; ++n, copy += width / 2) {
            std::reverse(copy, copy + width/2);
        }
    }

    return output;
}

template<class Reader>
StringVector parse_string_body(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    StringVector output(len);
    for (size_t i = 0; i < len; ++i) {
        auto str = parse_single_string(reader, leftovers);
        output.data[i] = str.value;
        output.encodings[i] = str.encoding;
        output.missing[i] = str.missing;
    }
    return output;
}

}

#endif
