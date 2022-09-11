#ifndef RDS2CPP_STRING_HPP
#define RDS2CPP_STRING_HPP

#include "utils.hpp"
#include "SEXPType.hpp"
#include "StringEncoding.hpp"
#include <string>

namespace rds2cpp {

struct StringInfo {
    std::string value;
    StringEncoding encoding;
    bool missing;
};

template<class Reader>
StringInfo parse_single_string(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto header = parse_header(reader, leftovers);
    std::reverse(header.begin(), header.end());
    if (header[0] != static_cast<unsigned>(SEXPType::CHAR)) {
        throw std::runtime_error("elements of a character vector should be CHARSXP");
    }

    StringInfo output;
    auto& enc = output.encoding;
    if (header[1] & (1 << (12 - 8 + 1))) {
        enc = StringEncoding::NONE;
    } else if (header[1] & (1 << (12 - 8 + 2))) {
        enc = StringEncoding::LATIN1;
    } else if (header[1] & (1 << (12 - 8 + 3))) {
        enc = StringEncoding::UTF8;
    } else if (header[2] & (1 << (12 - 16 + 6))) {
        enc = StringEncoding::ASCII;
    }

    // Getting the string length.
    uint32_t strlen = 0;
    bool ok = extract_up_to(reader, leftovers, 4, 
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
        output.missing= true;
    } else {
        output.missing= false;
        auto& str = output.value;
        bool ok = extract_up_to(reader, leftovers, strlen,
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                str.insert(str.end(), buffer, buffer + n);
            }
        );
        if (!ok) {
            throw std::runtime_error("failed to parse the string in a character vector");
        }
    }

    return output;
}

}

#endif
