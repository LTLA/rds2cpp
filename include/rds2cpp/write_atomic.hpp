#ifndef RDS2CPP_WRITE_ATOMIC_HPP
#define RDS2CPP_WRITE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"
#include "write_attributes.hpp"

namespace rds2cpp {

namespace atomic_internal {

template<class Vector>
void set_vector_header(Vector& vec, std::vector<unsigned char>& buffer) {
    buffer.clear();
    inject_header(vec, buffer);
    return;
}

template<class Vector, class Writer>
void write_integer_or_logical_body(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const Vector*>(obj);
    set_vector_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    constexpr size_t width = 4;
    size_t nbytes = len * width;

    if (little_endian()) {
        buffer.clear();
        buffer.insert(buffer.end(), ptr, ptr + nbytes);
        auto copy = buffer.data();
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
        writer.write(buffer.data(), buffer.size());
    } else {
        writer.write(ptr, nbytes);
    }

    write_attributes(vec.attributes, writer, buffer, shared);
}

}

template<class Writer>
void write_integer(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    return atomic_internal::write_integer_or_logical_body<IntegerVector>(obj, writer, buffer, shared);
}

template<class Writer>
void write_logical(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    return atomic_internal::write_integer_or_logical_body<LogicalVector>(obj, writer, buffer, shared);
}

template<class Writer>
void write_double(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const DoubleVector*>(obj);
    atomic_internal::set_vector_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    constexpr size_t width = 8;
    size_t nbytes = len * width;
    
    if (little_endian()) {
        buffer.clear();
        buffer.insert(buffer.end(), ptr, ptr + nbytes);
        auto copy = buffer.data();
        for (size_t n = 0; n < len; ++n, copy += width) {
            std::reverse(copy, copy + width);
        }
        writer.write(buffer.data(), buffer.size());
    } else {
        writer.write(ptr, nbytes);
    }

    write_attributes(vec.attributes, writer, buffer, shared);
}

template<class Writer>
void write_raw(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const RawVector*>(obj);
    atomic_internal::set_vector_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    writer.write(values.data(), len);
    write_attributes(vec.attributes, writer, buffer, shared);
}

template<class Writer>
void write_complex(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const ComplexVector*>(obj);
    atomic_internal::set_vector_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    auto ptr = reinterpret_cast<const unsigned char*>(values.data());
    constexpr size_t width = 16;
    size_t nbytes = len * width;

    if (little_endian()) {
        buffer.clear();
        buffer.insert(buffer.end(), ptr, ptr + nbytes);
        auto copy = buffer.data();
        for (size_t n = 0; n < len * 2; ++n, copy += width / 2) {
            std::reverse(copy, copy + width/2);
        }
        writer.write(buffer.data(), buffer.size());
    } else {
        writer.write(ptr, nbytes);
    }

    write_attributes(vec.attributes, writer, buffer, shared);
}

template<class Writer>
void write_string(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const StringVector*>(obj);
    atomic_internal::set_vector_header(vec, buffer);

    const auto& values = vec.data;
    auto len = values.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    if (len != vec.encodings.size()) {
        throw std::runtime_error("vectors of strings and encodings should have the same length");
    }
    if (len != vec.missing.size()) {
        throw std::runtime_error("vectors of strings and missingness should have the same length");
    }

    for (size_t i = 0; i < len; ++i) {
        write_single_string(vec.data[i], vec.encodings[i], vec.missing[i], writer, buffer);
    }
    write_attributes(vec.attributes, writer, buffer, shared);
}

}

#endif
