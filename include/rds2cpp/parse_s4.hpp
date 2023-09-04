#ifndef RDS2CPP_PARSE_S4_HPP
#define RDS2CPP_PARSE_S4_HPP

#include <cstdint>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo&);

template<class Source_>
PairList parse_pairlist_body(Source_&, const Header&, SharedParseInfo&);

template<class Source_>
S4Object parse_s4_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    if (!(header[2] & 0x2) || !(header[2] & 0x1) || !(header[1] & 0x1)) {
        throw std::runtime_error("S4 objects should have object, attribute, and gp-S4 bits set in header");
    }

    S4Object output;

    auto slot_header = parse_header(src);
    if (slot_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("slots of an S4 object should be stored as a pairlist");
    }

    auto slot_plist = parse_pairlist_body(src, slot_header, shared);
    size_t nslots = slot_plist.data.size();
    bool found_class = false;

    for (size_t s = 0; s < nslots; ++s) {
        if (!slot_plist.has_tag[s]) {
            throw std::runtime_error("all slots in an S4 object should be named");
        }
        auto& content = slot_plist.data[s];

        if (slot_plist.tag_names[s] != "class") {
            output.attributes.names.emplace_back(std::move(slot_plist.tag_names[s]));
            output.attributes.encodings.push_back(slot_plist.tag_encodings[s]);
            output.attributes.values.emplace_back(std::move(content));
        } else {
            if (found_class) {
                throw std::runtime_error("multiple class attributes detected in an S4 object");
            }
            found_class = true;

            if (content->type() != SEXPType::STR) {
                throw std::runtime_error("class attribute in an S4 object should be a character vector");
            }
            
            auto cls = static_cast<StringVector*>(content.get());
            if (cls->data.size() != 1) {
                throw std::runtime_error("class attribute in an S4 object should be a length-1 character vector");
            }
            output.class_name = cls->data[0];
            output.class_encoding = cls->encodings[0];

            if (cls->attributes.values.size() != 1 || cls->attributes.names[0] != "package" || cls->attributes.values[0]->type() != SEXPType::STR) {
                throw std::runtime_error("class attribute in an S4 object should have a 'package' character attribute");
            }
            auto pkg = static_cast<StringVector*>(cls->attributes.values[0].get());
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
} catch (std::exception& e) {
    throw traceback("failed to parse an S4 object's body", e);
}

}

#endif

