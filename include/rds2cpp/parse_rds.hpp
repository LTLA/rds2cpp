#ifndef RDS2CPP_PARSE_HPP
#define RDS2CPP_PARSE_HPP

#include <memory>
#include <stdexcept>
#include <cstdint>

#include "RdsFile.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"
#include "parse_object.hpp"

#include "byteme/byteme.hpp"
#include "sanisizer/sanisizer.hpp"

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
    std::size_t buffer_size = sanisizer::cap<std::size_t>(65536);
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

    RdsFile output;

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
            output.format_version = quick_integer<I<decltype(output.format_version)> >(src);
        } catch (std::exception& e) {
            throw traceback("failed to read the format version number from the RDS preamble", e);
        } 

        // Using int32_t for the versions as the R_Version macro operates with signed integers AFIACT.
        try {
            auto writer_version = quick_integer<std::int32_t>(src);
            output.writer_version.major = (writer_version >> 16);
            output.writer_version.minor = (writer_version >> 8) & 255;
            output.writer_version.patch = writer_version & 255;
        } catch (std::exception& e) {
            throw traceback("failed to read the writer version number from the RDS preamble", e);
        }

        try {
            auto reader_version = quick_integer<std::int32_t>(src);
            output.reader_version.major = (reader_version >> 16);
            output.reader_version.minor = (reader_version >> 8) & 255;
            output.reader_version.patch = reader_version & 255;
        } catch (std::exception& e) {
            throw traceback("failed to read the reader version number from the RDS preamble", e);
        }
    }

    // Reading this undocumented section about the string encoding.
    {
        std::int32_t encoding_length = 0;
        try {
            encoding_length = quick_integer<I<decltype(encoding_length)> >(src);
            if (encoding_length < 0) {
                throw std::runtime_error("encoding length should be non-negative");
            }
        } catch (std::exception& e) {
            throw traceback("failed to read the encoding length from the RDS preamble", e);
        }

        try {
            std::string encoding;
            encoding.reserve(encoding_length); // don't resize and use extract() on string::data, as that pointer is read-only AFAICT.
            for (I<decltype(encoding_length)> b = 0; b < encoding_length; ++b) {
                if (!src.advance()) {
                    throw empty_error();
                }
                encoding.push_back(as_char(src.get()));
            }
            output.encoding = string_encoding_from_name(encoding);
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
