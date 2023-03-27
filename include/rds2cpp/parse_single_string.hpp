#ifndef RDS2CPP_STRING_HPP
#define RDS2CPP_STRING_HPP

#include "utils_parse.hpp"
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
StringInfo parse_single_string(Reader& reader, std::vector<unsigned char>& leftovers) try {
    auto header = parse_header(reader, leftovers);
    std::reverse(header.begin(), header.end()); // make it little-endian for easier indexing.
    if (header[0] != static_cast<unsigned char>(SEXPType::CHAR)) {
        throw std::runtime_error("expected a CHARSXP representation for a string");
    }

    // Getting the string length; all strings are less than 2^31-1,
    // see https://cran.r-project.org/doc/manuals/r-release/R-ints.html#Long-vectors.
    uint32_t strlen = 0;
    extract_up_to(reader, leftovers, 4, 
        [&](const unsigned char* buffer, size_t n, size_t) -> void {
            for (size_t x = 0; x < n; ++x) {
                strlen <<= 8;
                strlen += buffer[x];
            }
        }
    );

    StringInfo output;
    output.missing = (strlen == static_cast<uint32_t>(-1));

    if (!output.missing) {
        auto& str = output.value;
        extract_up_to(reader, leftovers, strlen,
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                auto ptr = reinterpret_cast<const char*>(buffer);
                str.insert(str.end(), ptr, ptr + n);
            }
        );

        /* String encoding is stored in the gp field, from bits 12 to 27 in the header.
         * We make life easier by just accessing the relevant byte below, after adjusting
         * the start of the gp field for the skipped bytes. For more details, see
         * https://cran.r-project.org/doc/manuals/r-release/R-ints.html#Rest-of-header.
         */
        auto& enc = output.encoding;
        if (header[1] & (1 << (12 - 8 + 1))) {
            enc = StringEncoding::NONE;
        } else if (header[1] & (1 << (12 - 8 + 2))) {
            enc = StringEncoding::LATIN1;
        } else if (header[1] & (1 << (12 - 8 + 3))) {
            enc = StringEncoding::UTF8;
        } else if (header[2] & (1 << (12 - 16 + 6))) {
            enc = StringEncoding::ASCII;
        } else {
            enc = StringEncoding::UTF8; // fall back to UTF8 if no encoding can be detected.
        }
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse a single CHARSXP", e);
}

}

#endif
