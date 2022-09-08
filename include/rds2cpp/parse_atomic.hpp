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

    return new DoubleVector(std::move(output));
}

template<class Reader>
RawVector* parse_raw(Reader& reader, std::vector<unsigned char>& leftovers) {
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

    return new RawVector(std::move(output));
}

template<class Reader>
ComplexVector* parse_complex(Reader& reader, std::vector<unsigned char>& leftovers) {
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

    return new ComplexVector(std::move(output));
}

template<class Reader>
CharacterVector* parse_character(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    CharacterVector output(len);

    for (size_t i = 0; i < len; ++i) {
        std::array<unsigned char, 4> header;

        // Examining the header.
        bool ok = extract_up_to(reader, leftovers, 4, 
            [&](const unsigned char* buffer, size_t n, size_t i) -> void {
                std::copy(buffer, buffer + n, header.data() + i);
            }
        );
        if (!ok) {
            throw std::runtime_error("failed to parse the element header in a character vector");
        }

        std::reverse(header.begin(), header.end());
        if (static_cast<SEXPType>(header[0]) != CHAR) {
            throw std::runtime_error("elements of a character vector should be CHARSXP");
        }

        auto& enc = output.data[i].encoding;
        if (header[1] & (1 << (12 - 8 + 1))) {
            enc = String::NONE;
        } else if (header[1] & (1 << (12 - 8 + 2))) {
            enc = String::LATIN1;
        } else if (header[1] & (1 << (12 - 8 + 3))) {
            enc = String::UTF8;
        } else if (header[2] & (1 << (12 - 16 + 6))) {
            enc = String::ASCII;
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
            output.data[i].missing= true;
            continue;
        }

        auto& str = output.data[i].value;
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
