#ifndef RDS2CPP_PARSE_ATOMIC_HPP
#define RDS2CPP_PARSE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"

namespace rds2cpp {

template<class Vector, class Reader>
Vector* parse_integer_or_logical(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    Vector output(len);

    constexpr size_t width = 4;
    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    extract_up_to(reader, leftovers, width * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );

    // Flipping endianness.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    return new Vector(std::move(output));
}

template<class Reader>
IntegerVector* parse_integer(Reader& reader, std::vector<unsigned char>& leftovers) {
    return parse_integer_or_logical<IntegerVector>(reader, leftovers);
}

template<class Reader>
LogicalVector* parse_logical(Reader& reader, std::vector<unsigned char>& leftovers) {
    return parse_integer_or_logical<LogicalVector>(reader, leftovers);
}

template<class Reader>
DoubleVector* parse_double(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    DoubleVector output(len);

    constexpr size_t width = 8;
    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    extract_up_to(reader, leftovers, width * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );

    // Flipping endianness.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    return new DoubleVector(std::move(output));
}

template<class Reader>
RawVector* parse_raw(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    RawVector output(len);

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    extract_up_to(reader, leftovers, len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );

    return new RawVector(std::move(output));
}

template<class Reader>
ComplexVector* parse_complex(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    ComplexVector output(len);

    constexpr size_t width = 16;
    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    extract_up_to(reader, leftovers, width * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );

    // Flipping endianness for each double.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len * 2; ++n, copy += width / 2) {
            std::reverse(copy, copy + width/2);
        }
    }

    return new ComplexVector(std::move(output));
}

template<class Reader>
CharacterVector* parse_character(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    CharacterVector output(len);

    for (size_t i = 0; i < len; ++i) {
        // Mystery first 4 bytes, just throw them away.
        bool ok = extract_up_to(reader, leftovers, 4, [&](const unsigned char* buffer, size_t n, size_t i) -> void {});
        if (!ok) {
            throw std::runtime_error("failed to parse the mystery bytes in a character vector");
        }

        // Getting the string length.
        uint32_t strlen = 0;
        ok = extract_up_to(reader, leftovers, 4, 
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                for (size_t x = 0; x < n; ++x) {
                    strlen <<= 8;
                    strlen += buffer[x];
                }
            }
        );
        if (!ok) {
            throw std::runtime_error("failed to parse the string length in a character vector");
        }

        // Handle NAs.
        if (strlen == static_cast<uint32_t>(-1)) {
            output.data[i].first = true;
            continue;
        }

        auto& str = output.data[i].second;
        ok = extract_up_to(reader, leftovers, strlen,
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                str.insert(str.end(), buffer, buffer + n);
            }
        );
        if (!ok) {
            throw std::runtime_error("failed to parse the string in a character vector");
        }
    }

    return new CharacterVector(std::move(output));
}

}

#endif
