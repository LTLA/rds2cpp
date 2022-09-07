#ifndef RDS2CPP_PARSE_HPP
#define RDS2CPP_PARSE_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_object.hpp"

#include "byteme/SomeFileReader.hpp"

namespace rds2cpp {

struct Parsed {
    uint32_t format_version = 0;
    std::array<unsigned char, 3> writer_version;
    std::array<unsigned char, 3> reader_version;
    std::string encoding;
    std::shared_ptr<RObject> object;
};

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
    output.object = parse_object(reader, leftovers);

    return output;
}

}

#endif
