#ifndef RDS2CPP_PARSE_HPP
#define RDS2CPP_PARSE_HPP

#include <memory>
#include <algorithm>
#include <cstdint>

#include "RdsFile.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"
#include "parse_object.hpp"

#include "byteme/byteme.hpp"

/**
 * @file parse_rds.hpp
 *
 * @brief Parse an RDS file.
 */

namespace rds2cpp {

/**
 * @brief Options for `parse_rds()`.
 */
struct ParseRdsOptions {
    /**
     * Whether to read and parse the contents of the RDS file in parallel.
     */
    bool parallel = false;

    /**
     * Size of the buffer for storing read bytes before parsing.
     * Larger values improve speed at the cost of memory efficiency.
     */
    std::size_t buffer_size = 65536;
};

/**
 * Parse the contents of an RDS file.
 *
 * @tparam Reader_ A `byteme::Reader` class, or any class with a compatible interface.
 *
 * @param reader Instance of a `Reader` class, containing the contents of the RDS file.
 * @param options Further options for parsing.
 *
 * @return An `RdsFile` object containing the contents of the RDS file.
 */
template<class Reader_>
RdsFile parse_rds(Reader_& reader, const ParseRdsOptions& options) {
    std::unique_ptr<byteme::BufferedReader<unsigned char> > srcptr;
    if (options.parallel) {
        srcptr.reset(new byteme::SerialBufferedReader<unsigned char, Reader_*>(&reader, options.buffer_size));
    } else {
        srcptr.reset(new byteme::ParallelBufferedReader<unsigned char, Reader_*>(&reader, options.buffer_size));
    }
    auto& src = *srcptr; 
    RdsFile output(false);

    // Reading the header first. This is the first and only time that 
    // we need to do a src.valid() check, as we're using the current 
    // position of the source; in all other cases, it can be assumed
    // that the source needs to be advance()'d before get().
    {
        try {
            if (!src.valid()) {
                throw empty_error();
            }
            if (src.get() != 'X') {
                throw std::runtime_error("only RDS files in XDR format are currently supported");
            }

            if (!src.advance()) {
                throw empty_error();
            }
            if (src.get() != '\n') {
                throw std::runtime_error("only RDS files in XDR format are currently supported");
            }
        } catch (std::exception& e) {
            throw traceback("failed to read the header from the RDS preamble", e);
        }

        output.format_version = 0;
        try {
            for (int i = 0; i < 4; ++i) {
                if (!src.advance()) {
                    throw empty_error();
                }
                output.format_version <<= 8;
                output.format_version += src.get();
            }
        } catch (std::exception& e) {
            throw traceback("failed to read the format version number from the RDS preamble", e);
        } 

        // Just skipping the first byte for the R reader/writer versions...
        // unless we get up to a major version > 255, then we're in trouble.
        try {
            if (!src.advance()) {
                throw empty_error();
            }
            quick_extract(src, output.writer_version.size(), output.writer_version.data());
        } catch (std::exception& e) {
            throw traceback("failed to read the writer version number from the RDS preamble", e);
        }

        try {
            if (!src.advance()) {
                throw empty_error();
            }
            quick_extract(src, output.reader_version.size(), output.reader_version.data());
        } catch (std::exception& e) {
            throw traceback("failed to read the reader version number from the RDS preamble", e);
        }
    }

    // Reading this undocumented section about the string encoding.
    {
        size_t encoding_length = 0;
        try {
            for (int b = 0; b < 4; ++b) {
                if (!src.advance()) {
                    throw empty_error();
                }
                encoding_length <<= 8;
                encoding_length += src.get();
            }
        } catch (std::exception& e) {
            throw traceback("failed to read the encoding length from the RDS preamble", e);
        }

        try {
            output.encoding.reserve(encoding_length); // don't resize and use extract() on string::data, as that pointer is read-only AFAICT.
            for (size_t b = 0; b < encoding_length; ++b) {
                if (!src.advance()) {
                    throw empty_error();
                }
                output.encoding.push_back(as_char(src.get()));
            }
        } catch (std::exception& e) {
            throw traceback("failed to read the encoding string from the RDS preamble", e);
        }
    }

    // Now we can finally read the damn object.
    SharedParseInfo shared;
    output.object = parse_object(src, shared);
    output.environments = std::move(shared.environments);
    output.symbols = std::move(shared.symbols);
    output.external_pointers = std::move(shared.external_pointers);

    return output;
}

/**
 * Parse the contents of a Gzip-compressed RDS file.
 *
 * @param file Path to a Gzip-compressed RDS file.
 * @param options Further options for parsing.
 *
 * @return An `RdsFile` object containing the contents of `file`.
 */
inline RdsFile parse_rds(std::string file, const ParseRdsOptions& options) {
    byteme::GzipFileReader reader(file.c_str(), {});
    return parse_rds(reader, options);
}

/**
 * Typedef for back-compatibility.
 */
typedef RdsFile Parsed;

}

#endif
