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

    // Writing the class information as the first "attribute" among the S4 slots.
    // We do this manually because I can't figure out how to register a symbol for 'class' and 'package' while keeping everything const.
    {
        write_symbol("class", StringEncoding::ASCII, bufwriter, shared); // We'll guess the encoding of the 'class' string. 

        Header details;
        details[0] = 0;
        details[1] = 0;
        details[2] = 0x2; // i.e., this also has attributes.
        details[3] = static_cast<unsigned char>(SEXPType::STR);
        bufwriter.write(details.data(), details.size());
        inject_length(1, bufwriter);
        write_single_string(ptr->class_name, ptr->class_encoding, bufwriter);

        // Adding the package attribute.
        {
            inject_next_pairlist_header(true, bufwriter);
            write_symbol("package", StringEncoding::ASCII, bufwriter, shared); // similarly guessing the encoding of the 'package' string.

            Header details;
            details[0] = 0;
            details[1] = 0;
            details[2] = 0;
            details[3] = static_cast<unsigned char>(SEXPType::STR);
            bufwriter.write(details.data(), details.size());
            inject_length(1, bufwriter);
            write_single_string(ptr->package_name, ptr->package_encoding, bufwriter);

            inject_header(SEXPType::NILVALUE_, bufwriter);
        }
    }

    // Forcibly writing all attributes with NIL termination.
    write_attributes_body(ptr->attributes, bufwriter, shared);
}

}

#endif
