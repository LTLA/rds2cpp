#ifndef RDS2CPP_WRITE_PAIRLIST_HPP
#define RDS2CPP_WRITE_PAIRLIST_HPP

#include <vector>
#include <memory>

#include "SharedWriteInfo.hpp"
#include "write_object.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"

namespace rds2cpp {

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared);

template<class Writer>
void write_pairlist(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    auto ptr = static_cast<const PairList*>(object);
    const auto& values = ptr->data;
    const auto& has_tag = ptr->has_tag;
    const auto& tag_names = ptr->tag_names;
    const auto& tag_encodings = ptr->tag_encodings;
    size_t n = values.size();
    bool has_attr = !ptr->attributes.names.empty();

    for (size_t i = 0; i < n; ++i) {
        buffer.clear();
        buffer.insert(buffer.end(), 2, 0);

        // see logic in parse_pairlist.
        unsigned char x = 0;
        if (i == 0 && has_attr) {
            x |= 0x2;
        }
        if (has_tag[i]) {
            x |= 0x4;
        }
        buffer.push_back(x);

        buffer.push_back(static_cast<unsigned char>(SEXPType::LIST));
        writer.write(buffer.data(), buffer.size());

        if (i == 0 && has_attr) {
            write_attributes(ptr->attributes, writer, buffer, shared);
        }
        if (has_tag[i]) {
            shared.write_symbol(tag_names[i], tag_encodings[i], writer, buffer);
        }

        write_object(values[i].get(), writer, buffer, shared); 
    }

    buffer.clear();
    inject_header(SEXPType::NILVALUE_, buffer);
    writer.write(buffer.data(), buffer.size());
    return;
}

}

#endif
