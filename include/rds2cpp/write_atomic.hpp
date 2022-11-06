#ifndef RDS2CPP_WRITE_ATOMIC_HPP
#define RDS2CPP_WRITE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

template<class Writer>
void write_integer(const IntegerVector& vec, Writer& writer, std::vector<unsigned char>& buffer) {
    buffer.resize(4);
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = (vec.attributes.names.empty() ? 0 : 2);
    buffer[3] = static_cast<unsigned char>(SEXPType::INT); // cast from enum should be safe, as SEXPTypes are also unsigned chars.

    const auto& values = vec.data;
    auto len = values.size();
    inject_integer(len, buffer);

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

#endif
