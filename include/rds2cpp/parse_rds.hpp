#ifndef RDS2CPP_PARSE_HPP
#define RDS2CPP_PARSE_HPP

#include <memory>
#include <algorithm>
#include <cstdint>

#include "RdsFile.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"
#include "parse_object.hpp"

#include "byteme/SomeFileReader.hpp"

/**
 * @file parse_rds.hpp
 *
 * @brief Parse an RDS file.
 */

namespace rds2cpp {

/**
 * Parse the contents of an RDS file.
 *
 * @tparam Reader A [`byteme::Reader`](https://ltla.github.io/byteme) class.
 *
 * @param reader Instance of a `Reader` class, containing the contents of the RDS file.
 *
 * @return An `RdsFile` object containing the contents of the RDS file.
 */
template<class Reader>
RdsFile parse_rds(Reader& reader) {
    RdsFile output(false);

    std::vector<unsigned char> leftovers;

    // Reading the header first.
    {
        std::vector<unsigned char> accumulated;
        try {
            extract_up_to(reader, leftovers, 14,
                [&](const unsigned char* buffer, size_t n, size_t) -> void {
                    accumulated.insert(accumulated.end(), buffer, buffer + n);
                }
            );
        } catch (std::exception& e) {
            throw traceback("failed to read the header from the RDS preamble", e);
        }

        if (static_cast<char>(accumulated[0]) != 'X' && static_cast<char>(accumulated[1]) != '\n') {
            throw std::runtime_error("only RDS files in XDR format are currently supported");
        }

        output.format_version = 0;
        for (size_t pos = 2; pos < 6; ++pos) {
            output.format_version <<= 8;
            output.format_version += accumulated[pos];
        }

        // Just skipping the first byte for the versions... unless we get up
        // to a major version > 255, then we're in trouble.
        for (size_t pos = 7; pos < 10; ++pos) {
           output.writer_version[pos - 7] = accumulated[pos];
        }

        for (size_t pos = 11; pos < 14; ++pos) {
           output.reader_version[pos - 11] = accumulated[pos];
        }
    }

    // Reading this undocumented section about the string encoding.
    {
        size_t encoding_length = 0;
        try {
            extract_up_to(reader, leftovers, 4,
                [&](const unsigned char* buffer, size_t n, size_t) -> void {
                    for (size_t b = 0; b < n; ++b) {
                        encoding_length <<= 8;
                        encoding_length += buffer[b];
                    }
                }
            );
        } catch (std::exception& e) {
            throw traceback("failed to read the encoding length from the RDS preamble", e);
        }

        try {
            extract_up_to(reader, leftovers, encoding_length,
                [&](const unsigned char* buffer, size_t n, size_t) -> void {
                    output.encoding.insert(output.encoding.end(), buffer, buffer + n);
                }
            );
        } catch (std::exception& e) {
            throw traceback("failed to read the encoding string from the RDS preamble", e);
        }
    }

    // Now we can finally read the damn object.
    SharedParseInfo shared;
    output.object = parse_object(reader, leftovers, shared);
    output.environments = std::move(shared.environments);
    output.symbols = std::move(shared.symbols);
    output.external_pointers = std::move(shared.external_pointers);

    return output;
}

/**
 * Parse the contents of an RDS file.
 *
 * @param file Path to an RDS file.
 *
 * @return An `RdsFile` object containing the contents of `file`.
 */
inline RdsFile parse_rds(std::string file) {
    byteme::SomeFileReader reader(file.c_str());
    return parse_rds(reader);
}

/**
 * Typedef for back-compatibility.
 */
typedef RdsFile Parsed;

}

#endif
