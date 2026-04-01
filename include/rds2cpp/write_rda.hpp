#ifndef RDS2CPP_WRITE_RDA_HPP
#define RDS2CPP_WRITE_RDA_HPP

#include <vector>
#include <cstddef>

#include "byteme/byteme.hpp"
#include "sanisizer/sanisizer.hpp"

#include "RdaFile.hpp"
#include "RObject.hpp"
#include "utils_write.hpp"
#include "write_object.hpp"
#include "SharedWriteInfo.hpp"

/**
 * @file write_rda.hpp
 *
 * @brief Write an RDA file.
 */

namespace rds2cpp {

/**
 * @brief Options for `write_rda()`.
 */
struct WriteRdaOptions {
    /**
     * Whether to write the contents of the RDA file in parallel with serialization.
     */
    bool parallel = false;

    /**
     * Size of the buffer for storing serialized bytes before writing.
     * Larger values improve speed at the cost of memory efficiency.
     */
    std::size_t buffer_size = sanisizer::cap<std::size_t>(65536);
};

/**
 * Convert an R object into the RDA format and write it to a specified output.
 *
 * @tparam Writer A `byteme::Writer` class, or any class with a compatible interface.
 *
 * @param info Information about the RDA file to be written, including a pointer to a valid `RObject`.
 * @param writer Instance of a `Writer` class, where the RDA file is to be written.
 * @param options Further options.
 */
template<class Writer>
void write_rda(const RdaFile& info, Writer& writer, const WriteRdaOptions& options) {
    std::unique_ptr<byteme::BufferedWriter<unsigned char> > bufwriter;
    if (options.parallel) {
        bufwriter.reset(new byteme::ParallelBufferedWriter<unsigned char, Writer*>(&writer, options.buffer_size));
    } else {
        bufwriter.reset(new byteme::SerialBufferedWriter<unsigned char, Writer*>(&writer, options.buffer_size));
    }

    bufwriter->write("RDX3\nX\n");
    inject_integer<std::int32_t>(info.format_version, *bufwriter);
    write_version(info.writer_version, *bufwriter);
    write_version(info.reader_version, *bufwriter);

    const std::string encoding = string_encoding_to_name(info.encoding);
    inject_integer<std::int32_t>(sanisizer::cast<std::int32_t>(encoding.size()), *bufwriter);
    bufwriter->write(encoding);

    SharedWriteInfo shared(info.symbols, info.environments, info.external_pointers);

    // We need to write it out as a tagged pairlist but we can't move the unique_ptrs into a temporary PairList while keeping things const. 
    // So we'll just do it manually, by duplicating the logic in write_pairlist(). 
    for (const auto& entry : info.objects) {
        Header details;
        details[0] = 0;
        details[1] = 0;
        details[2] = 0x4; // i.e., has tag.
        details[3] = static_cast<unsigned char>(SEXPType::LIST);
        bufwriter->write(details.data(), details.size());
        write_symbol(&(entry.name), *bufwriter, shared);
        write_object(entry.value.get(), *bufwriter, shared); 
    }

    inject_header(SEXPType::NILVALUE_, *bufwriter);
}

/**
 * Write an R object to a Gzip-compressed RDA file.
 *
 * @param info Information about the RDA file to be written, including a pointer to a valid `RObject`.
 * @param path Path to the output file.
 * @param options Further options.
 */
inline void write_rda(const RdaFile& info, const char* path, const WriteRdaOptions& options) {
    byteme::GzipFileWriter writer(path, {});
    write_rda(info, writer, options);
}

/**
 * Write an R object to a Gzip-compressed RDA file.
 *
 * @param info Information about the RDA file to be written, including a pointer to a valid `RObject`.
 * @param path Path to the output file.
 * @param options Further options.
 */
inline void write_rda(const RdaFile& info, std::string path, const WriteRdaOptions& options) {
    write_rda(info, path.c_str(), options);
}


}

#endif
