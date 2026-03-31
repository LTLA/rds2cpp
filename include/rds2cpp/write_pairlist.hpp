#ifndef RDS2CPP_WRITE_PAIRLIST_HPP
#define RDS2CPP_WRITE_PAIRLIST_HPP

#include <vector>
#include <memory>

#include "SharedWriteInfo.hpp"
#include "write_object.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"

namespace rds2cpp {

template<class BufferedWriter_>
void write_object(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared);

template<class BufferedWriter_>
void write_pairlist(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    auto ptr = static_cast<const PairList*>(object);
    const bool has_attr = !ptr->attributes.empty();
    bool start = true;

    for (const auto& entry : ptr->data) {
        Header details;
        details[0] = 0;
        details[1] = 0;

        // see logic in parse_pairlist.
        unsigned char x = 0;
        if (start && has_attr) {
            x |= 0x2;
        }
        if (entry.tag.has_value()) {
            x |= 0x4;
        }
        details[2] = x;

        details[3] = static_cast<unsigned char>(SEXPType::LIST);
        bufwriter.write(details.data(), details.size());

        if (start && has_attr) {
            write_attributes(ptr->attributes, bufwriter, shared);
        }

        if (entry.tag.has_value()) {
            shared.write_symbol(&(*(entry.tag)), bufwriter);
        }

        write_object(entry.value.get(), bufwriter, shared); 
        start = false;
    }

    inject_header(SEXPType::NILVALUE_, bufwriter);
}

}

#endif
