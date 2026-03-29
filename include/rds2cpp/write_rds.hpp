#ifndef RDS2CPP_WRITE_RDS_HPP
#define RDS2CPP_WRITE_RDS_HPP

#include <vector>

#include "RdsFile.hpp"
#include "RObject.hpp"
#include "utils_write.hpp"
#include "write_object.hpp"
#include "SharedWriteInfo.hpp"

#include "byteme/GzipFileWriter.hpp"

/**
 * @file write_rds.hpp
 *
 * @brief Write an RDS file.
 */

namespace rds2cpp {

/**
 * Convert an R object into the RDS format and write it to a specified output.
 *
 * @tparam Writer A `byteme::Writer` class, or any class with a compatible interface.
 *
 * @param info Information about the RDS file to be written, including a pointer to a valid `RObject`.
 * @param writer Instance of a `Writer` class, where the RDS file is to be written.
 */
template<class Writer>
void write_rds(const RdsFile& info, Writer& writer) {
    std::vector<unsigned char> buffer;
    inject_string("X\n", 2, buffer);
    inject_integer(info.format_version, buffer);

    // Mimic the behavior of the R_Version macro.
    inject_integer(
        sanisizer::product_unsafe<std::int32_t>(info.writer_version.major, 65536) +
        sanisizer::product_unsafe<std::int32_t>(info.writer_version.minor, 256) +
        static_cast<std::int32_t>(info.writer_version.patch),
        buffer
    );

    inject_integer(
        sanisizer::product_unsafe<std::int32_t>(info.reader_version.major, 65536) +
        sanisizer::product_unsafe<std::int32_t>(info.reader_version.minor, 256) +
        static_cast<std::int32_t>(info.reader_version.patch),
        buffer
    );

    const std::string encoding = string_encoding_to_name(info.encoding);
    inject_integer(sanisizer::cast<std::int32_t>(encoding.size()), buffer);
    inject_string(encoding.c_str(), encoding.size(), buffer);

    writer.write(buffer.data(), buffer.size());

    SharedWriteInfo shared(info.symbols, info.environments, info.external_pointers);
    write_object(info.object.get(), writer, buffer, shared); 

    return;
}

/**
 * Write an R object to a Gzip-compressed RDS file.
 *
 * @param info Information about the RDS file to be written, including a pointer to a valid `RObject`.
 * @param path Path to the output file.
 */
inline void write_rds(const RdsFile& info, const char* path) {
    byteme::GzipFileWriter writer(path, {});
    write_rds(info, writer);
    return;
}

/**
 * Write an R object to a Gzip-compressed RDS file.
 *
 * @param info Information about the RDS file to be written, including a pointer to a valid `RObject`.
 * @param path Path to the output file.
 */
inline void write_rds(const RdsFile& info, std::string path) {
    write_rds(info, path.c_str());
    return;
}


}

#endif
