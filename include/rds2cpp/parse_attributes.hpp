#ifndef RDS2CPP_PARSE_ATTRIBUTES_HPP
#define RDS2CPP_PARSE_ATTRIBUTES_HPP

#include <vector>
#include <memory>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_pairlist.hpp"

namespace rds2cpp {

template<class Reader>
std::shared_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&);

inline bool has_attributes(const Header& header) {
    return (header[2] & 0x2);
}

template<class Reader>
void parse_attributes(Reader& reader, std::vector<unsigned char>& leftovers, RObject& object) {
    auto header = parse_header(reader, leftovers);
    if (header[3] != LIST) {
        throw std::runtime_error("attributes should be a pairlist");
    }

    std::shared_ptr<PairList> plist;
    plist.reset(parse_pairlist(reader, leftovers, header));

    const auto& tags = plist->tags;
    for (const auto& t : tags) {
        if (!t.first || t.second.missing) {
            throw std::runtime_error("all attributes should be named");
        }
        object.attribute_encodings.push_back(t.second.encoding);
        object.attribute_names.push_back(t.second.value);
    }

    std::swap(object.attribute_values, plist->data);
    return;
}

}

#endif
