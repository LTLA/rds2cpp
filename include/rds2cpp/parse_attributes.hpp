#ifndef RDS2CPP_PARSE_ATTRIBUTES_HPP
#define RDS2CPP_PARSE_ATTRIBUTES_HPP

#include <vector>
#include <memory>

#include "RObject.hpp"
#include "Shared.hpp"
#include "utils.hpp"

namespace rds2cpp {

template<class Reader>
PairList parse_pairlist_body(Reader&, std::vector<unsigned char>&, const Header&, Shared&);

inline bool has_attributes(const Header& header) {
    return (header[2] & 0x2);
}

template<class Reader>
void parse_attributes_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, Attributes& output, Shared& shared) {
    auto plist = parse_pairlist_body(reader, leftovers, header, shared);

    size_t nnodes = plist.data.size();
    for (size_t t = 0; t < nnodes; ++t) {
        if (!plist.has_tag[t]) {
            throw std::runtime_error("all attributes should be named");
        }
    }

    output.values.swap(plist.data);
    output.names.swap(plist.tag_names);
    output.encodings.swap(plist.tag_encodings);
    return;
}

template<class Reader>
void parse_attributes(Reader& reader, std::vector<unsigned char>& leftovers, Attributes& output, Shared& shared) {
    auto header = parse_header(reader, leftovers);
    if (header[3] != static_cast<unsigned>(SEXPType::LIST)) {
        throw std::runtime_error("attributes should be a pairlist");
    }
    parse_attributes_body(reader, leftovers, header, output, shared);
    return;
}

}

#endif
