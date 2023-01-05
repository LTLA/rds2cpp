#ifndef RDS2CPP_WRITE_S4_HPP
#define RDS2CPP_WRITE_S4_HPP

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
void write_s4(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    auto ptr = static_cast<const S4Object*>(object);

    buffer.clear();
    buffer.push_back(0);
    buffer.push_back(0x1); // see logic in parse_s4.
    buffer.push_back(0x1 | 0x2);
    buffer.push_back(static_cast<unsigned char>(SEXPType::S4));

    // Adding the header.
    inject_next_pairlist_header(true, buffer);
    writer.write(buffer.data(), buffer.size());

    // Injecting the name of the tag.
    // We'll guess the encoding of the 'class' string as UTF-8.
    shared.write_symbol("class", StringEncoding::UTF8, writer, buffer); 

    // Writing the class information.
    StringVector class_info;
    class_info.data.push_back(ptr->class_name);
    class_info.encodings.push_back(ptr->class_encoding);
    class_info.missing.push_back(false);

    class_info.attributes.names.push_back("package");
    class_info.attributes.encodings.push_back(StringEncoding::UTF8); // Guessing the encoding of the 'package' string itself.
    auto pkg_ptr = new StringVector;
    class_info.attributes.values.emplace_back(pkg_ptr);

    pkg_ptr->data.push_back(ptr->package_name);
    pkg_ptr->encodings.push_back(ptr->package_encoding);
    pkg_ptr->missing.push_back(false);

    write_string(&class_info, writer, buffer, shared);

    // Writing the remaining slots.
    write_attributes(ptr->attributes, writer, buffer, shared);
}

}

#endif
