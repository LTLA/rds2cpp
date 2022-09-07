#ifndef RDS2CPP_UTILS_HPP
#define RDS2CPP_UTILS_HPP

#include <algorithm>
#include <vector>
#include <cstdint>

namespace rds2cpp {

template<class Reader, class Function>
bool extract_up_to(Reader& reader, std::vector<unsigned char>& leftovers, size_t expected, Function fun) {
    size_t processed = std::min(expected, leftovers.size());
    fun(leftovers.data(), processed, 0);
    std::copy(leftovers.begin() + processed, leftovers.end(), leftovers.begin());
    leftovers.resize(leftovers.size() - processed);

    bool remaining = true;
    while (processed < expected) {
        if (!remaining) {
            return false;
        }

        remaining = reader();
        const unsigned char * buffer = reader.buffer();
        size_t available = reader.available();

        size_t required = expected - processed;
        size_t usable = std::min(required, available);
        fun(buffer, usable, processed);
        processed += usable;

        if (processed == expected) {
            leftovers.insert(leftovers.end(), buffer + usable, buffer + available);
            break;
        }
    }

    return true;
}

template<class Reader>
size_t get_length(Reader& reader, std::vector<unsigned char>& leftovers) {
    uint32_t initial = 0;

    bool ok = extract_up_to(reader, leftovers, 4, 
        [&](const unsigned char* buffer, size_t n, size_t) -> void {
            for (size_t b = 0; b < n; ++b) {
                initial <<= 8;
                initial += buffer[b];
            }
        }
    );
    if (!ok) {
        throw std::runtime_error("failed to extract vector length");
    }

    if (initial != static_cast<uint32_t>(-1)) {
        return initial;
    }

    // Hack to deal with large lengths. 
    uint64_t full = 0;
    ok = extract_up_to(reader, leftovers, 8, 
        [&](const unsigned char* buffer, size_t n, size_t) -> void {
            for (size_t b = 0; b < n; ++b) {
                full <<= 8;
                full += buffer[b];
            }
        }
    );
    if (!ok) {
        throw std::runtime_error("failed to extract large vector length");
    }

    return full;
}

inline bool little_endian() {
    const uint32_t value = 1;
    auto lsb = reinterpret_cast<const unsigned char *>(&value);
    return (*lsb == 1);
}

}

#endif
