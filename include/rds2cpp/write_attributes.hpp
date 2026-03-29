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

template<class BufferedWriter_>
bool write_attributes(const Attributes& attr, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const auto nattr = attr.names.size();
    if (!nattr) {
        return false;
    }

    if (nattr != attr.encodings.size()) {
        throw std::runtime_error("vectors of attribute names and encodings should have the same length");
    }
    if (nattr != attr.values.size()) {
        throw std::runtime_error("vectors of attribute names and values should have the same length");
    }

    for (I<decltype(nattr)> a = 0; a < nattr; ++a) {
        inject_next_pairlist_header(true, bufwriter);
        shared.write_symbol(attr.names[a], attr.encodings[a], bufwriter);
        write_object(attr.values[a].get(), bufwriter, shared);
    }

    inject_header(SEXPType::NILVALUE_, bufwriter);
    return true;
}

}

#endif
