#ifndef RDS2CPP_PARSE_HPP
#define RDS2CPP_PARSE_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>

#include "RObject.hpp"
#include "utils.hpp"
#include "Shared.hpp"
#include "parse_object.hpp"

#include "byteme/SomeFileReader.hpp"

/**
 * @file parse_rds.hpp
 *
 * @brief Parse an RDS file.
 */

namespace rds2cpp {

/**
 * @brief Contents of the parsed RDS file.
 */
struct Parsed {
    /**
     * Version of the RDS format.
     */
    uint32_t format_version = 0;

    /**
     * R version used to write the file as major-minor-patch integers.
     */
    std::array<unsigned char, 3> writer_version;

    /**
     * Minimum R version required to read the file as major-minor-patch integers.
     */
    std::array<unsigned char, 3> reader_version;

    /**
     * Encoding required to read the file.
     */
    std::string encoding;

    /**
     * The unserialized object.
     */
    std::unique_ptr<RObject> object;

    /**
     * All environments inside the file.
     * This can be referenced by the `index` in `EnvironmentIndex`.
     */
    std::vector<Environment> environments;

    /**
     * All symbols inside the file.
     * This can be referenced by the `index` in `SymbolIndex`.
     */
    std::vector<Symbol> symbols;
};

/**
 * Parse the contents of an RDS file.
 *
 * @param file Path to an RDS file.
 *
 * @return A `Parsed` object containing the contents of `file`.
 */
inline Parsed parse_rds(std::string file) {
    byteme::SomeFileReader reader(file.c_str());
    Parsed output;

    std::vector<unsigned char> leftovers;
    bool remaining = true;

    // Reading the header first.
    {
        std::vector<unsigned char> accumulated;
        bool ok = extract_up_to(reader, leftovers, 14,
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                accumulated.insert(accumulated.end(), buffer, buffer + n);
            }
        );
        if (!ok) {
            throw std::runtime_error("RDS file is too short to contain the header");
        }

        if (static_cast<char>(accumulated[0]) != 'X' && static_cast<char>(accumulated[1]) != '\n') {
            throw std::runtime_error("only RDS files in XDR format are currently supported");
        }

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
        bool ok = extract_up_to(reader, leftovers, 4,
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                for (size_t b = 0; b < n; ++b) {
                    encoding_length <<= 8;
                    encoding_length += buffer[b];
                }
            }
        );
        if (!ok) {
            throw std::runtime_error("RDS file is too short to contain the encoding length");
        }

        ok = extract_up_to(reader, leftovers, encoding_length,
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                output.encoding.insert(output.encoding.end(), buffer, buffer + n);
            }
        );
        if (!ok) {
            throw std::runtime_error("RDS file is too short to contain the encoding string");
        }
    }

    // Now we can finally read the damn object.
    Shared shared;
    output.object = parse_object(reader, leftovers, shared);
    output.environments = std::move(shared.environments);
    output.symbols = std::move(shared.symbols);

    return output;
}

}

#endif
