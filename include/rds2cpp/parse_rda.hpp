#ifndef RDS2CPP_PARSE_RDA_HPP
#define RDS2CPP_PARSE_RDA_HPP

#include <memory>
#include <stdexcept>
#include <cstdint>

#include "RdaFile.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"
#include "parse_pairlist.hpp"

#include "byteme/byteme.hpp"
#include "sanisizer/sanisizer.hpp"

/**
 * @file parse_rda.hpp
 *
 * @brief Parse an RDA (a.k.a., RData) file.
 */

namespace rds2cpp {

/**
 * @brief Options for `parse_rda()`.
 */
struct ParseRdaOptions {
    /**
     * Whether to read and parse the contents of the RDA file in parallel.
     */
    bool parallel = false;

    /**
     * Size of the buffer for storing read bytes before parsing.
     * Larger values improve speed at the cost of memory efficiency.
     */
    std::size_t buffer_size = sanisizer::cap<std::size_t>(65536);
};

/**
 * Parse the contents of an RDA file.
 *
 * @tparam Reader_ A `byteme::Reader` class, or any class with a compatible interface.
 *
 * @param reader Instance of a `Reader` class, containing the contents of the RDA file.
 * @param options Further options for parsing.
 *
 * @return An `RdaFile` object containing the contents of the RDA file.
 */
template<class Reader_>
RdaFile parse_rda(Reader_& reader, const ParseRdaOptions& options) {
    std::unique_ptr<byteme::BufferedReader<unsigned char> > srcptr;
    if (options.parallel) {
        srcptr.reset(new byteme::SerialBufferedReader<unsigned char, Reader_*>(&reader, options.buffer_size));
    } else {
        srcptr.reset(new byteme::ParallelBufferedReader<unsigned char, Reader_*>(&reader, options.buffer_size));
    }
    auto& src = *srcptr; 

    RdaFile output;

    // Reading the header first. This is the first and only time that 
    // we need to do a src.valid() check, as we're using the current 
    // position of the source; in all other cases, it can be assumed
    // that the source needs to be advance()'d before get().
    {
        try {
            if (!src.valid()) {
                throw empty_error();
            }

            std::string header;
            header += as_char(src.get());
            for (int i = 0; i < 4; ++i) {
                if (!src.advance()) {
                    throw empty_error();
                }
                header += as_char(src.get());
            }
            if (header != "RDX2\n" && header != "RDX3\n") {
                throw std::runtime_error("unsupported format are currently supported");
            }

            header.clear();
            for (int i = 0; i < 2; ++i) {
                if (!src.advance()) {
                    throw empty_error();
                }
                header += as_char(src.get());
            }
            if (header != "X\n") {
                throw std::runtime_error("only RDA files in XDR format are currently supported");
            }
        } catch (std::exception& e) {
            throw traceback("failed to read the header from the RDA preamble", e);
        }

        output.format_version = 0;
        try {
            output.format_version = quick_integer<I<decltype(output.format_version)> >(src);
        } catch (std::exception& e) {
            throw traceback("failed to read the format version number from the RDA preamble", e);
        } 

        try {
            output.writer_version = parse_version(src);
        } catch (std::exception& e) {
            throw traceback("failed to read the writer version number from the RDA preamble", e);
        }

        try {
            output.reader_version = parse_version(src);
        } catch (std::exception& e) {
            throw traceback("failed to read the reader version number from the RDA preamble", e);
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
            throw traceback("failed to read the encoding length from the RDA preamble", e);
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
            throw traceback("failed to read the encoding string from the RDA preamble", e);
        }
    }

    // Now we can finally read the damn object.
    SharedParseInfo shared;

    auto details = parse_header(src);
    auto sexp_type = details[3];
    if (sexp_type != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected RDA file to contain a pairlist");
    }

    auto payload = parse_pairlist_body(src, details, shared);
    output.objects.reserve(payload->data.size());
    for (auto& x : payload->data) {
        if (!x.tag.has_value()) {
            throw std::runtime_error("expected RDA file to contain a tagged pairlist");
        }
        output.objects.emplace_back(std::move(*(x.tag)), std::move(x.value));
    }

    output.environments = std::move(shared.environments);
    output.symbols = std::move(shared.symbols);
    output.external_pointers = std::move(shared.external_pointers);

    return output;
}

/**
 * Parse the contents of a Gzip-compressed RDA file.
 *
 * @param file Path to a Gzip-compressed RDA file.
 * @param options Further options for parsing.
 *
 * @return An `RdaFile` object containing the contents of `file`.
 */
inline RdaFile parse_rda(std::string file, const ParseRdaOptions& options) {
    byteme::GzipFileReader reader(file.c_str(), {});
    return parse_rda(reader, options);
}

}

#endif
