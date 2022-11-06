#ifndef RDS2CPP_WRITE_RDS_HPP
#define RDS2CPP_WRITE_RDS_HPP

#include <vector>

#include "byteme/GzipFileWriter.hpp"

#include "parse_rds.hpp"
#include "utils_write.hpp"
#include "write_object.hpp"

namespace rds2cpp {

void set_typical_defaults(Parsed& parsed) {
    parsed.format_version = 3;

    parsed.writer_version[0] = 4;
    parsed.writer_version[1] = 2;
    parsed.writer_version[2] = 0;

    parsed.reader_version[0] = 3;
    parsed.reader_version[1] = 5;
    parsed.reader_version[2] = 0;

    parsed.encoding = "UTF-8";
}

template<class Writer>
void write_rds(const Parsed& parsed, Writer& writer) {
    std::vector<unsigned char> buffer;
    inject_string("X\n", 2, buffer);
    inject_integer(parsed.format_version, buffer);

    buffer.push_back(0);
    for (auto x : parsed.writer_version) {
        buffer.push_back(x);
    }

    buffer.push_back(0);
    for (auto x : parsed.reader_version) {
        buffer.push_back(x);
    }

    size_t encoding_len = parsed.encoding.size();
    inject_integer(encoding_len, buffer);
    inject_string(parsed.encoding.c_str(), encoding_len, buffer);
    writer.write(buffer.data(), buffer.size());

    write_object(parsed.object.get(), writer, buffer); 

    return;
}

inline void write_rds(const Parsed& parsed, std::string path) {
    byteme::GzipFileWriter writer(path);
    write_rds(parsed, writer);
}

}

#endif
