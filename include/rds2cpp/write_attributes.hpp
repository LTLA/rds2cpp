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
void write_attributes_body(const std::vector<Attribute>& attributes, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    for (const auto& attr : attributes) {
        inject_next_pairlist_header(true, bufwriter);
        write_symbol(&(attr.name), bufwriter, shared);
        write_object(attr.value.get(), bufwriter, shared);
    }

    inject_header(SEXPType::NILVALUE_, bufwriter);
}

template<class BufferedWriter_>
void write_attributes(const std::vector<Attribute>& attributes, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    // If there weren't any attributes, we skip the pairlist creation.
    // It is assumed that the attribute bit was unset in such cases.
    if (attributes.empty()) {
        return;
    }
    write_attributes_body(attributes, bufwriter, shared);
}

}

#endif
