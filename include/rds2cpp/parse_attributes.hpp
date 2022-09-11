#ifndef RDS2CPP_PARSE_ATTRIBUTES_HPP
#define RDS2CPP_PARSE_ATTRIBUTES_HPP

#include <vector>
#include <memory>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_pairlist.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&);

inline bool has_attributes(const Header& header) {
    return (header[2] & 0x2);
}

template<class Reader>
void parse_attributes(Reader& reader, std::vector<unsigned char>& leftovers, RObject& object) {
    auto header = parse_header(reader, leftovers);
    if (header[3] != static_cast<unsigned>(SEXPType::LIST)) {
        throw std::runtime_error("attributes should be a pairlist");
    }

    auto plist = parse_pairlist_body(reader, leftovers, header);

    size_t nnodes = plist.data.size();
    for (size_t t = 0; t < nnodes; ++t) {
        if (!plist.has_tag[t]) {
            throw std::runtime_error("all attributes should be named");
        }
    }

    object.attribute_values.swap(plist.data);
    object.attribute_names.swap(plist.tag_names);
    object.attribute_encodings.swap(plist.tag_encodings);
    return;
}

}

#endif
