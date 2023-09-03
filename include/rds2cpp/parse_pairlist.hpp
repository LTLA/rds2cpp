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

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo&);

namespace pairlist_internal {

template<class Source_>
void recursive_parse(Source_& src, PairList& output, const Header& header, SharedParseInfo& shared) {
    bool has_attr = header[2] & 0x2;
    bool has_tag = header[2] & 0x4;

    if (has_attr) {
        parse_attributes(src, output.attributes, shared);
    }

    output.has_tag.push_back(has_tag);
    if (has_tag) {
        auto header = parse_header(src);
        size_t sindex;

        if (header[3] == static_cast<unsigned char>(SEXPType::SYM)) {
            auto sdx = parse_symbol_body(src, shared);
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

    try {
        output.data.push_back(parse_object(src, shared));
    } catch (std::exception& e) {
        if (output.tag_names.back().empty()) {
            throw traceback("failed to parse unnamed pairlist element " + std::to_string(output.tag_names.size()), e);
        } else {
            throw traceback("failed to parse pairlist element '" + output.tag_names.back() + "'", e);
        }
    }

    auto next_header = parse_header(src);
    if (next_header[3] == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        return;
    } else if (next_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected a terminator or the next pairlist node");
    }

    recursive_parse(src, output, next_header, shared);
    return;
}

}

template<class Source_>
PairList parse_pairlist_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    PairList output;
    pairlist_internal::recursive_parse(src, output, header, shared);
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse a pairlist body", e);
}

}

#endif
