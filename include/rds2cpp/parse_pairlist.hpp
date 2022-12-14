#ifndef RDS2CPP_PARSE_PAIRLIST_HPP
#define RDS2CPP_PARSE_PAIRLIST_HPP

#include <cstdint>
#include <vector>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"

#include "parse_single_string.hpp"
#include "parse_attributes.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&, SharedParseInfo&);

namespace pairlist_internal {

template<class Reader>
void recursive_parse(Reader& reader, std::vector<unsigned char>& leftovers, PairList& output, const Header& header, SharedParseInfo& shared) {
    bool has_attr = header[2] & 0x2;
    bool has_tag = header[2] & 0x4;

    if (has_attr) {
        parse_attributes(reader, leftovers, output.attributes, shared);
    }

    output.has_tag.push_back(has_tag);
    if (has_tag) {
        auto header = parse_header(reader, leftovers);
        size_t sindex;

        if (header[3] == static_cast<unsigned char>(SEXPType::SYM)) {
            auto sdx = parse_symbol_body(reader, leftovers, shared);
            sindex = sdx.index;
        } else if (header[3] == static_cast<unsigned char>(SEXPType::REF)) {
            sindex = shared.get_symbol_index(header);
        } else {
            throw std::runtime_error("expected a SYMSXP for a pairlist tag");
        }

        const auto& sym = shared.symbols[sindex];
        output.tag_names.push_back(sym.name);
        output.tag_encodings.push_back(sym.encoding);
    } else {
        auto n = output.tag_names.size() + 1;
        output.tag_names.resize(n);
        output.tag_encodings.resize(n);
    }

    output.data.push_back(parse_object(reader, leftovers, shared));

    auto next_header = parse_header(reader, leftovers);
    if (next_header[3] == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        return;
    } else if (next_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected a terminator or the next pairlist node");
    }

    recursive_parse(reader, leftovers, output, next_header, shared);
    return;
}

}

template<class Reader>
PairList parse_pairlist_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, SharedParseInfo& shared) {
    PairList output;
    pairlist_internal::recursive_parse(reader, leftovers, output, header, shared);
    return output;
}

}

#endif
