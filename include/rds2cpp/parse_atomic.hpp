#ifndef RDS2CPP_PARSE_ATOMIC_HPP
#define RDS2CPP_PARSE_ATOMIC_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"

namespace rds2cpp {

template<class Reader>
IntegerVector* parse_integer(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    IntegerVector output(len);

    auto ptr = reinterpret_cast<unsigned char*>(output.data.data());
    extract_up_to(reader, leftovers, 4 * len,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            std::copy(buffer, buffer + n, ptr + i);
        }
    );

    // Flipping endianness.
    if (little_endian()) {
        auto copy = ptr;
        for (size_t n = 0; n < len; ++n, copy += 4) {
            std::reverse(copy, copy + 4);
        }
    }

    return new IntegerVector(std::move(output));
}

}

#endif
