#ifndef RDS2CPP_WRITE_SINGLE_STRING_HPP
#define RDS2CPP_WRITE_SINGLE_STRING_HPP

#include "utils_write.hpp"
#include "SEXPType.hpp"
#include "StringEncoding.hpp"
#include <string>

namespace rds2cpp {

template<class Writer>
void write_single_string(const std::string& value, StringEncoding encoding, bool missing, Writer& writer, std::vector<unsigned char>& buffer) {
    buffer.clear();
    buffer.resize(4);
    buffer[3] = static_cast<unsigned char>(SEXPType::CHAR);

    if (missing) {
        inject_integer(-1, buffer);
    } else {
        if (value.size() > 2147483647) {
            throw std::runtime_error("strings should be less than 2^31-1 characters in length");
        }

        // Reverse of the logic in parse_single_string.
        // Note that the positions on the buffer aren't reversed here.
        switch (encoding) {
            case StringEncoding::NONE:
                buffer[2] = 1 << (12 - 8 + 1);
                break;
            case StringEncoding::LATIN1:
                buffer[2] = 1 << (12 - 8 + 2);
                break;
            case StringEncoding::UTF8:
                buffer[2] = 1 << (12 - 8 + 3);
                break;
            case StringEncoding::ASCII:
                buffer[1] = 1 << (12 - 16 + 6);
                break;
            default:
                throw std::runtime_error("unsupported string encoding");
        }

        inject_integer(value.size(), buffer);
        inject_string(value.c_str(), value.size(), buffer);
    }

    writer.write(buffer.data(), buffer.size());
    return;
}

}

#endif
