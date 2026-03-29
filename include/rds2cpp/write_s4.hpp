#ifndef RDS2CPP_WRITE_S4_HPP
#define RDS2CPP_WRITE_S4_HPP

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
void write_s4(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    auto ptr = static_cast<const S4Object*>(object);

    Header details;
    details[0] = 0;
    details[1] = 0x1; // see logic in parse_s4.
    details[2] = 0x1 | 0x2;
    details[3] = static_cast<unsigned char>(SEXPType::S4);
    bufwriter.write(details.data(), details.size());

    // Adding the header.
    inject_next_pairlist_header(true, bufwriter);

    // Injecting the name of the tag.
    // We'll guess the encoding of the 'class' string as UTF-8.
    shared.write_symbol("class", StringEncoding::UTF8, bufwriter); 

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

    write_string(&class_info, bufwriter, shared);

    // Writing the remaining slots.
    write_attributes(ptr->attributes, bufwriter, shared);
}

}

#endif
