#ifndef RDS2CPP_PARSE_S4_HPP
#define RDS2CPP_PARSE_S4_HPP

#include <cstdint>
#include <vector>

#include "RObject.hpp"
#include "utils.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&);

template<class Reader>
S4Object parse_s4_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header) {
    if (!(header[2] & 0x2) || !(header[2] & 0x1) || !(header[1] & 0x1)) {
        throw std::runtime_error("S4 objects should have object, attribute, and gp-S4 bits set in header");
    }

    S4Object output;

    auto slot_header = parse_header(reader, leftovers);
    if (slot_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("slots of an S4 object should be stored as a pairlist");
    }

    auto slot_plist = parse_pairlist_body(reader, leftovers, slot_header);
    size_t nslots = slot_plist.data.size();
    bool found_class = false;

    for (auto s = 0; s < nslots; ++s) {
        if (!slot_plist.has_tag[s]) {
            throw std::runtime_error("all slots in an S4 object should be named");
        }
        auto& content = slot_plist.data[s];

        if (slot_plist.tag_names[s] != "class") {
            output.attribute_names.emplace_back(std::move(slot_plist.tag_names[s]));
            output.attribute_encodings.push_back(slot_plist.tag_encodings[s]);
            output.attribute_values.emplace_back(std::move(content));
        } else {
            if (found_class) {
                throw std::runtime_error("multiple class attributes detected in an S4 object");
            }
            found_class = true;

            if (content->sexp_type != SEXPType::STR) {
                throw std::runtime_error("class attribute in an S4 object should be a character vector");
            }
            
            auto cls = static_cast<CharacterVector*>(content.get());
            if (cls->data.size() != 1) {
                throw std::runtime_error("class attribute in an S4 object should be a length-1 character vector");
            }
            output.class_name = cls->data[0];
            output.class_encoding = cls->encodings[0];

            if (cls->attribute_values.size() != 1 || cls->attribute_names[0] != "package" || cls->attribute_values[0]->sexp_type != SEXPType::STR) {
                throw std::runtime_error("class attribute in an S4 object should have a 'package' character attribute");
            }
            auto pkg = static_cast<CharacterVector*>(cls->attribute_values[0].get());
            if (pkg->data.size() != 1) {
                throw std::runtime_error("package attribute in an S4 object should be a length-1 character vector");
            }
            output.package_name = pkg->data[0];
            output.package_encoding = pkg->encodings[0];
        }
    }

    if (!found_class) {
        throw std::runtime_error("failed to detect class attribute in an S4 object");
    }

    return output;
}

}

#endif

