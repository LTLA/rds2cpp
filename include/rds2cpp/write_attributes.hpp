#ifndef RDS2CPP_WRITE_ATTRIBUTES_HPP
#define RDS2CPP_WRITE_ATTRIBUTES_HPP

#include <vector>
#include <memory>

#include "write_object.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"

namespace rds2cpp {

template<class Writer>
void write_attributes(const Attributes& attr, Writer& writer, std::vector<unsigned char>& buffer) {
    size_t nattr = attr.names.size();
    if (!nattr) {
        return;
    }

    if (nattr != attr.encodings.size()) {
        throw std::runtime_error("vectors of attribute names and encodings should have the same length");
    }
    if (nattr != attr.values.size()) {
        throw std::runtime_error("vectors of attribute names and values should have the same length");
    }

    for (size_t a = 0; a < nattr; ++a) {
        buffer.clear();

        // Adding the header.
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(4);
        buffer.push_back(static_cast<unsigned char>(SEXPType::LIST));

        // Injecting the name of the tag.
        // TODO: this requires proper symbol reference handling.
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(static_cast<unsigned char>(SEXPType::SYM));

        writer.write(buffer.data(), buffer.size());
        write_single_string(attr.names[a], attr.encodings[a], false, writer, buffer);
        write_object(attr.values[a].get(), writer, buffer);
    }

    buffer.clear();
    buffer.push_back(0);
    buffer.push_back(0);
    buffer.push_back(0);
    buffer.push_back(static_cast<unsigned char>(SEXPType::NILVALUE_));
    writer.write(buffer.data(), buffer.size());
}

}

#endif
