#ifndef RDS2CPP_PARSE_S4_HPP
#define RDS2CPP_PARSE_S4_HPP

#include <vector>
#include <stdexcept>
#include <memory>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo&);

template<class Source_>
std::unique_ptr<PairList> parse_pairlist_body(Source_&, const Header&, SharedParseInfo&);

template<class Source_>
std::unique_ptr<S4Object> parse_s4_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    if (!(header[2] & 0x2) || !(header[2] & 0x1) || !(header[1] & 0x1)) {
        throw std::runtime_error("S4 objects should have object, attribute, and gp-S4 bits set in header");
    }

    auto slot_header = parse_header(src);
    if (slot_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("slots of an S4 object should be stored as a pairlist");
    }

    auto slot_plist = parse_pairlist_body(src, slot_header, shared);

    bool found_class = false;
    auto output = std::make_unique<S4Object>();
    output->attributes.reserve(slot_plist->data.empty() ? 0 : slot_plist->data.size() - 1); // ignoring the class attribute. 

    for (auto& entry : slot_plist->data) {
        if (!entry.tag.has_value()) {
            throw std::runtime_error("all slots in an S4 object should be named");
        }

        if (shared.symbols[entry.tag->index].name != "class") {
            output->attributes.emplace_back(std::move(*(entry.tag)), std::move(entry.value));
            continue;
        }

        if (found_class) {
            throw std::runtime_error("multiple class attributes detected in an S4 object");
        }
        found_class = true;

        if (entry.value->type() != SEXPType::STR) {
            throw std::runtime_error("class attribute in an S4 object should be a character vector");
        }

        auto cls = static_cast<StringVector*>(entry.value.get());
        if (cls->data.size() != 1) {
            throw std::runtime_error("class attribute in an S4 object should be a length-1 character vector");
        }
        if (!(cls->data[0].value.has_value())) {
            throw std::runtime_error("class attribute in an S4 object should not be a missing string");
        }
        output->class_name = *(cls->data[0].value);
        output->class_encoding = cls->data[0].encoding;

        if (cls->attributes.size() != 1 ||
            shared.symbols[cls->attributes[0].name.index].name != "package" ||
            cls->attributes[0].value->type() != SEXPType::STR
        ) {
            throw std::runtime_error("class attribute in an S4 object should have a 'package' character attribute");
        }
        auto pkg = static_cast<StringVector*>(cls->attributes[0].value.get());
        if (pkg->data.size() != 1) {
            throw std::runtime_error("package attribute in an S4 object should be a length-1 character vector");
        }
        if (!(pkg->data[0].value.has_value())) {
            throw std::runtime_error("package attribute in an S4 object should not be a missing string");
        }
        output->package_name = *(pkg->data[0].value);
        output->package_encoding = pkg->data[0].encoding;
    }

    if (!found_class) {
        throw std::runtime_error("failed to detect class attribute in an S4 object");
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an S4 object's body", e);
    return std::unique_ptr<S4Object>();
}

}

#endif
