#ifndef RDS2CPP_WRITE_SINGLE_STRING_HPP
#define RDS2CPP_WRITE_SINGLE_STRING_HPP

#include "utils_write.hpp"
#include "SEXPType.hpp"
#include "StringEncoding.hpp"
#include <string>

namespace rds2cpp {

inline void set_encoding_bit(StringEncoding encoding, Header& details) {
    // Reverse of the logic in parse_single_string.
    // Note that the positions on the header aren't reversed here.
    switch (encoding) {
        case StringEncoding::NONE:
            details[2] = 1 << (12 - 8 + 1);
            break;
        case StringEncoding::LATIN1:
            details[2] = 1 << (12 - 8 + 2);
            break;
        case StringEncoding::UTF8:
            details[2] = 1 << (12 - 8 + 3);
            break;
        case StringEncoding::ASCII:
            details[1] = 1 << (12 - 16 + 6);
            break;
        default:
            throw std::runtime_error("unsupported string encoding");
    }
}

template<class BufferedWriter_>
void write_single_string(const std::string& value, StringEncoding encoding, BufferedWriter_& bufwriter) {
    Header details;
    details[0] = 0;
    details[1] = 0;
    details[2] = 0;
    set_encoding_bit(encoding, details);
    details[3] = static_cast<unsigned char>(SEXPType::CHAR);
    bufwriter.write(details.data(), details.size());

    if (value.size() > 2147483647) {
        throw std::runtime_error("strings should be less than 2^31-1 characters in length");
    }
    inject_integer<std::int32_t, std::int32_t>(value.size(), bufwriter);
    bufwriter.write(value);
}

// Missing string overload, where the encoding is still respected.
template<class BufferedWriter_>
void write_single_string(StringEncoding encoding, BufferedWriter_& bufwriter) {
    Header details;
    details[0] = 0;
    details[1] = 0;
    details[2] = 0;
    set_encoding_bit(encoding, details);
    details[3] = static_cast<unsigned char>(SEXPType::CHAR);
    bufwriter.write(details.data(), details.size());
    inject_integer<std::int32_t, std::int32_t>(-1, bufwriter);
}

}

#endif
