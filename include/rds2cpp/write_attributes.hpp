#ifndef RDS2CPP_WRITE_ATTRIBUTES_HPP
#define RDS2CPP_WRITE_ATTRIBUTES_HPP

#include <vector>
#include <memory>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "SEXPType.hpp"
#include "write_object.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"

namespace rds2cpp {

template<class Writer>
bool write_attributes(const Attributes& attr, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    size_t nattr = attr.names.size();
    if (!nattr) {
        return false;
    }

    if (nattr != attr.encodings.size()) {
        throw std::runtime_error("vectors of attribute names and encodings should have the same length");
    }
    if (nattr != attr.values.size()) {
        throw std::runtime_error("vectors of attribute names and values should have the same length");
    }

    for (size_t a = 0; a < nattr; ++a) {
        buffer.clear();
        inject_next_pairlist_header(true, buffer);
        writer.write(buffer.data(), buffer.size());

        shared.write_symbol(attr.names[a], attr.encodings[a], writer, buffer);
        write_object(attr.values[a].get(), writer, buffer, shared);
    }

    buffer.clear();
    inject_header(SEXPType::NILVALUE_, buffer);
    writer.write(buffer.data(), buffer.size());

    return true;
}

}

#endif
