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

template<class Vector_, class BufferedWriter_>
void write_integer_or_logical_body(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const Vector_*>(obj);
    inject_header(vec, bufwriter);

    const auto& values = vec.data;
    const auto len = sanisizer::cast<std::size_t>(values.size());
    inject_length(len, bufwriter);

    constexpr std::size_t width = 4;
    if (little_endian()) {
        for (auto x : values) {
            auto ptr = reinterpret_cast<unsigned char*>(&x);
            std::reverse(ptr, ptr + width);
            bufwriter.write(ptr, width);
        }
    } else {
        auto ptr = reinterpret_cast<const unsigned char*>(values.data());
        const auto nbytes = sanisizer::product_unsafe<std::size_t>(len, width); // must be safe, otherwise 'values' would have never been allocated.
        bufwriter.write(ptr, nbytes);
    }

    write_attributes(vec.attributes, bufwriter, shared);
}

template<class BufferedWriter_>
void write_integer(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    return write_integer_or_logical_body<IntegerVector>(obj, bufwriter, shared);
}

template<class BufferedWriter_>
void write_logical(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    return write_integer_or_logical_body<LogicalVector>(obj, bufwriter, shared);
}

template<class BufferedWriter_>
void write_double(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const DoubleVector*>(obj);
    inject_header(vec, bufwriter);

    const auto& values = vec.data;
    const auto len = sanisizer::cast<std::size_t>(values.size());
    inject_length(len, bufwriter);

    constexpr std::size_t width = 8;
    if (little_endian()) {
        for (auto x : values) {
            auto ptr = reinterpret_cast<unsigned char*>(&x);
            std::reverse(ptr, ptr + width);
            bufwriter.write(ptr, width);
        }
    } else {
        auto ptr = reinterpret_cast<const unsigned char*>(values.data());
        const auto nbytes = sanisizer::product_unsafe<std::size_t>(len, width); // must be safe, otherwise 'values' would never have been allocated.
        bufwriter.write(ptr, nbytes);
    }

    write_attributes(vec.attributes, bufwriter, shared);
}

template<class BufferedWriter_>
void write_raw(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const RawVector*>(obj);
    inject_header(vec, bufwriter);

    const auto& values = vec.data;
    const auto len = sanisizer::cast<std::size_t>(values.size());
    inject_length(len, bufwriter);

    bufwriter.write(values.data(), len);
    write_attributes(vec.attributes, bufwriter, shared);
}

template<class BufferedWriter_>
void write_complex(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const ComplexVector*>(obj);
    inject_header(vec, bufwriter);

    const auto& values = vec.data;
    const auto len = sanisizer::cast<std::size_t>(values.size());
    inject_length(len, bufwriter);

    if (little_endian()) {
        constexpr std::size_t width = 8;
        for (auto x : values) {
            {
                auto rx = x.real();
                auto rptr = reinterpret_cast<unsigned char*>(&rx);
                std::reverse(rptr, rptr + width);
                bufwriter.write(rptr, width);
            }
            {
                auto ix = x.imag();
                auto iptr = reinterpret_cast<unsigned char*>(&ix);
                std::reverse(iptr, iptr + width);
                bufwriter.write(iptr, width);
            }
        }
    } else {
        constexpr std::size_t width = 16;
        auto ptr = reinterpret_cast<const unsigned char*>(values.data());
        const auto nbytes = sanisizer::product_unsafe<std::size_t>(len, width); // must be safe, otherwise 'values' would never be allocated.
        bufwriter.write(ptr, nbytes);
    }

    write_attributes(vec.attributes, bufwriter, shared);
}

template<class BufferedWriter_>
void write_string(const RObject* obj, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const auto& vec = *static_cast<const StringVector*>(obj);
    inject_header(vec, bufwriter); 

    const auto& values = vec.data;
    const auto len = sanisizer::cast<std::size_t>(values.size());
    inject_length(len, bufwriter);

    if (len != vec.encodings.size()) {
        throw std::runtime_error("vectors of strings and encodings should have the same length");
    }
    if (len != vec.missing.size()) {
        throw std::runtime_error("vectors of strings and missingness should have the same length");
    }

    for (I<decltype(len)> i = 0; i < len; ++i) {
        write_single_string(vec.data[i], vec.encodings[i], vec.missing[i], bufwriter);
    }
    write_attributes(vec.attributes, bufwriter, shared);
}

}

#endif
