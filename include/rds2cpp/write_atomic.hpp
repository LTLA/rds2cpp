#ifndef RDS2CPP_WRITE_ATOMIC_HPP
#define RDS2CPP_WRITE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"

namespace rds2cpp {

namespace atomic_internal {

template<class Vector>
void set_header(Vector& vec, std::vector<unsigned char>& buffer) {
    buffer.resize(4);
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = (vec.attributes.names.empty() ? 0 : 2);
    buffer[3] = static_cast<unsigned char>(vec.type()); // cast from enum should be safe, as SEXPTypes are also unsigned chars.
}

template<class Vector, class Writer>
void write_integer_or_logical_body(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    const auto& vec = *reinterpret_cast<const Vector*>(obj);
    set_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    constexpr size_t width = 4;
    size_t sofar = buffer.size();
    buffer.insert(buffer.end(), ptr, ptr + len * width);

    if (little_endian()) {
        auto copy = buffer.data() + sofar;
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    writer.write(buffer.data(), buffer.size());
}

}

template<class Writer>
void write_integer(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    return atomic_internal::write_integer_or_logical_body<IntegerVector>(obj, writer, buffer);
}

template<class Writer>
void write_logical(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    return atomic_internal::write_integer_or_logical_body<LogicalVector>(obj, writer, buffer);
}

template<class Writer>
void write_double(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    const auto& vec = *reinterpret_cast<const DoubleVector*>(obj);
    atomic_internal::set_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    constexpr size_t width = 8;
    size_t sofar = buffer.size();
    buffer.insert(buffer.end(), ptr, ptr + len * width);
    
    if (little_endian()) {
        auto copy = buffer.data() + sofar;
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
    }

    writer.write(buffer.data(), buffer.size());
}

template<class Writer>
void write_raw(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    const auto& vec = *reinterpret_cast<const RawVector*>(obj);
    atomic_internal::set_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    size_t sofar = buffer.size();
    buffer.insert(buffer.end(), ptr, ptr + len);

    writer.write(buffer.data(), buffer.size());
}

template<class Writer>
void write_complex(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    const auto& vec = *reinterpret_cast<const ComplexVector*>(obj);
    atomic_internal::set_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    size_t sofar = buffer.size();
    constexpr size_t width = 16;
    buffer.insert(buffer.end(), ptr, ptr + len * width);

    // Flipping endianness for each double.
    if (little_endian()) {
        auto copy = buffer.data() + sofar;
        for (size_t n = 0; n < len * 2; ++n, copy += width / 2) {
            std::reverse(copy, copy + width/2);
        }
    }

    writer.write(buffer.data(), buffer.size());
}

template<class Writer>
void write_string(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
    const auto& vec = *reinterpret_cast<const StringVector*>(obj);
    atomic_internal::set_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);

    for (size_t i = 0; i < len; ++i) {
        write_single_string(vec.data[i], vec.encodings[i], vec.missing[i], buffer);
    }

    writer.write(buffer.data(), buffer.size());
}


}

#endif
