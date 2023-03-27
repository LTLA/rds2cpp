#ifndef RDS2CPP_UTILS_PARSE_HPP
#define RDS2CPP_UTILS_PARSE_HPP

#include <algorithm>
#include <vector>
#include <cstdint>
#include <memory>

namespace rds2cpp {

template<class Reader, class Function>
void extract_up_to(Reader& reader, std::vector<unsigned char>& leftovers, size_t expected, Function fun) {
    size_t processed = std::min(expected, leftovers.size());
    fun(leftovers.data(), processed, 0);
    std::copy(leftovers.begin() + processed, leftovers.end(), leftovers.begin());
    leftovers.resize(leftovers.size() - processed);

    bool remaining = true;
    while (processed < expected) {
        if (!remaining) {
            throw std::runtime_error("no more bytes to read");
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
}

inline std::runtime_error traceback(std::string base, const std::exception& e) {
    return std::runtime_error(base + "\n  - " + e.what());
}

template<class Reader>
size_t get_length(Reader& reader, std::vector<unsigned char>& leftovers) {
    uint32_t initial = 0;
    try {
        extract_up_to(reader, leftovers, 4, 
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                for (size_t b = 0; b < n; ++b) {
                    initial <<= 8;
                    initial += buffer[b];
                }
            }
        );
    } catch (std::exception& e) {
        throw traceback("failed to extract vector length", e);
    }

    if (initial != static_cast<uint32_t>(-1)) {
        return initial;
    }

    // Hack to deal with large lengths. 
    uint64_t full = 0;
    try {
        extract_up_to(reader, leftovers, 8, 
            [&](const unsigned char* buffer, size_t n, size_t) -> void {
                for (size_t b = 0; b < n; ++b) {
                    full <<= 8;
                    full += buffer[b];
                }
            }
        );
    } catch (std::exception& e) {
        throw traceback("failed to extract large vector length", e);
    }

    return full;
}

inline bool little_endian() {
    const uint32_t value = 1;
    auto lsb = reinterpret_cast<const unsigned char *>(&value);
    return (*lsb == 1);
}

typedef std::array<unsigned char, 4> Header;

template<class Reader>
Header parse_header(Reader& reader, std::vector<unsigned char>& leftovers) try {
    Header details;
    extract_up_to(reader, leftovers, 4,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            for (size_t b = 0; b < n; ++b, ++i) {
                details[i] = buffer[b];
            }
        }
    );
    return details;
} catch (std::exception& e) {
    throw traceback("failed to parse the R object header", e);
}

template<class Pointer, class Object>
void pointerize(Pointer& ptr, Object obj) {
    ptr.reset(new Object(std::move(obj)));
    return;
}

}

#endif
