#ifndef RDS2CPP_WRITE_S4_HPP
#define RDS2CPP_WRITE_S4_HPP

#include <vector>
#include <memory>

#include "write_object.hpp"
#include "utils_write.hpp"
#include "write_single_string.hpp"

namespace rds2cpp {

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer);

template<class Writer>
void write_s4(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer) {
    auto ptr = static_cast<const S4Object*>(object);

    buffer.clear();
    buffer.push_back(0);
    buffer.push_back(0x1); // see logic in parse_s4.
    buffer.push_back(0x1 | 0x2);
    buffer.push_back(static_cast<unsigned char>(SEXPType::S4));

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
    write_single_string("class", StringEncoding::UTF8, false, writer, buffer); // guessing the encoding of the 'class' string itself.

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

    write_string(&class_info, writer, buffer);

    // Writing the remaining slots.
    write_attributes(ptr->attributes, writer, buffer);
}

}

#endif
