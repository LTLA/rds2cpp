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
    const auto& values = ptr->data;
    const auto& has_tag = ptr->has_tag;
    const auto& tag_names = ptr->tag_names;
    const auto& tag_encodings = ptr->tag_encodings;
    const auto n = values.size();
    const bool has_attr = !ptr->attributes.names.empty();

    for (I<decltype(n)> i = 0; i < n; ++i) {
        Header details;
        details[0] = 0;
        details[1] = 0;

        // see logic in parse_pairlist.
        unsigned char x = 0;
        if (i == 0 && has_attr) {
            x |= 0x2;
        }
        if (has_tag[i]) {
            x |= 0x4;
        }
        details[2] = x;

        details[3] = static_cast<unsigned char>(SEXPType::LIST);
        bufwriter.write(details.data(), details.size());

        if (i == 0 && has_attr) {
            write_attributes(ptr->attributes, bufwriter, shared);
        }
        if (has_tag[i]) {
            shared.write_symbol(tag_names[i], tag_encodings[i], bufwriter);
        }

        write_object(values[i].get(), bufwriter, shared); 
    }

    inject_header(SEXPType::NILVALUE_, bufwriter);
}

}

#endif
