#ifndef RDS2CPP_WRITE_RDS_HPP
#define RDS2CPP_WRITE_RDS_HPP

#include <vector>
#include <cstddef>

#include "byteme/byteme.hpp"
#include "sanisizer/sanisizer.hpp"

#include "RdsFile.hpp"
#include "RObject.hpp"
#include "utils_write.hpp"
#include "write_object.hpp"
#include "SharedWriteInfo.hpp"

/**
 * @file write_rds.hpp
 *
 * @brief Write an RDS file.
 */

namespace rds2cpp {

/**
 * @brief Options for `write_rds()`.
 */
struct WriteRdsOptions {
    /**
     * Whether to write the contents of the RDS file in parallel with serialization.
     */
    bool parallel = false;

    /**
     * Size of the buffer for storing serialized bytes before writing.
     * Larger values improve speed at the cost of memory efficiency.
     */
    std::size_t buffer_size = sanisizer::cap<std::size_t>(65536);
};

/**
 * Convert an R object into the RDS format and write it to a specified output.
 *
 * @tparam Writer A `byteme::Writer` class, or any class with a compatible interface.
 *
 * @param info Information about the RDS file to be written, including a pointer to a valid `RObject`.
 * @param writer Instance of a `Writer` class, where the RDS file is to be written.
 */
template<class Writer>
void write_rds(const RdsFile& info, Writer& writer, const WriteRdsOptions& options) {
    std::unique_ptr<byteme::BufferedWriter<unsigned char> > bufwriter;
    if (options.parallel) {
        bufwriter.reset(new byteme::ParallelBufferedWriter<unsigned char, Writer*>(&writer, options.buffer_size));
    } else {
        bufwriter.reset(new byteme::SerialBufferedWriter<unsigned char, Writer*>(&writer, options.buffer_size));
    }

    bufwriter->write("X\n");
    inject_integer<std::int32_t>(info.format_version, *bufwriter);

    // Mimic the behavior of the R_Version macro.
    inject_integer<std::int32_t, std::int32_t>(
        sanisizer::product_unsafe<std::int32_t>(info.writer_version.major, 65536) +
        sanisizer::product_unsafe<std::int32_t>(info.writer_version.minor, 256) +
        static_cast<std::int32_t>(info.writer_version.patch),
        *bufwriter
    );

    inject_integer<std::int32_t, std::int32_t>(
        sanisizer::product_unsafe<std::int32_t>(info.reader_version.major, 65536) +
        sanisizer::product_unsafe<std::int32_t>(info.reader_version.minor, 256) +
        static_cast<std::int32_t>(info.reader_version.patch),
        *bufwriter
    );

    const std::string encoding = string_encoding_to_name(info.encoding);
    inject_integer<std::int32_t>(sanisizer::cast<std::int32_t>(encoding.size()), *bufwriter);
    bufwriter->write(encoding);

    SharedWriteInfo shared(info.symbols, info.environments, info.external_pointers);
    write_object(info.object.get(), *bufwriter, shared); 
}

/**
 * Write an R object to a Gzip-compressed RDS file.
 *
 * @param info Information about the RDS file to be written, including a pointer to a valid `RObject`.
 * @param path Path to the output file.
 * @param options Further options.
 */
inline void write_rds(const RdsFile& info, const char* path, const WriteRdsOptions& options) {
    byteme::GzipFileWriter writer(path, {});
    write_rds(info, writer, options);
}

/**
 * Write an R object to a Gzip-compressed RDS file.
 *
 * @param info Information about the RDS file to be written, including a pointer to a valid `RObject`.
 * @param path Path to the output file.
 * @param options Further options.
 */
inline void write_rds(const RdsFile& info, std::string path, const WriteRdsOptions& options) {
    write_rds(info, path.c_str(), options);
}


}

#endif
