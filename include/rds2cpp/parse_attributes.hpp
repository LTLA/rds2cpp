#ifndef RDS2CPP_PARSE_ATTRIBUTES_HPP
#define RDS2CPP_PARSE_ATTRIBUTES_HPP

#include <vector>
#include <memory>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<PairList> parse_pairlist_body(Source_&, const Header&, SharedParseInfo&);

inline bool has_attributes(const Header& header) {
    return (header[2] & 0x2);
}

template<class Source_>
void parse_attributes_body(Source_& src, const Header& header, std::vector<Attribute>& output, SharedParseInfo& shared) try {
    auto plist = parse_pairlist_body(src, header, shared);
    output.reserve(plist->data.size());

    for (auto& entry : plist->data) {
        if (!(entry.tag.has_value())) {
            throw std::runtime_error("all attributes should be named");
        }
        output.emplace_back(std::move(*(entry.tag)), std::move(entry.value));
    }

} catch (std::exception& e) {
    throw traceback("failed to parse attribute contents", e);
}

template<class Source_>
void parse_attributes(Source_& src, std::vector<Attribute>& output, SharedParseInfo& shared) try {
    auto header = parse_header(src);
    if (header[3] != static_cast<unsigned>(SEXPType::LIST)) {
        throw std::runtime_error("attributes should be a pairlist");
    }
    parse_attributes_body(src, header, output, shared);
} catch (std::exception& e) {
    throw traceback("failed to parse attributes", e);
}

}

#endif
