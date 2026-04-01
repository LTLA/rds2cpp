#ifndef RDS2CPP_PARSE_PAIRLIST_HPP
#define RDS2CPP_PARSE_PAIRLIST_HPP

#include <cstddef>
#include <vector>
#include <stdexcept>

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

    if (has_tag) {
        auto header = parse_header(src);
        std::size_t sindex;

        if (header[3] == static_cast<unsigned char>(SEXPType::SYM)) {
            auto sdx = parse_symbol_body(src, shared);
            sindex = sdx->index;
        } else if (header[3] == static_cast<unsigned char>(SEXPType::REF)) {
            sindex = extract_symbol_index(header, shared);
        } else {
            throw std::runtime_error("expected a SYMSXP for a pairlist tag");
        }

        try {
            // Similarly, we need to copy sym.name rather than making a reference,
            // to ensure that the sym.name is still valid for emitting errors.
            output.data.emplace_back(SymbolIndex(sindex), parse_object(src, shared));
        } catch (std::exception& e) {
            throw traceback("failed to parse pairlist element '" + shared.symbols[sindex].name + "'", e);
        }

    } else {
        try {
            output.data.emplace_back(parse_object(src, shared));
        } catch (std::exception& e) {
            throw traceback("failed to parse unnamed pairlist element " + std::to_string(output.data.size()), e);
        }
    }

    auto next_header = parse_header(src);
    if (next_header[3] == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        return;
    } else if (next_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected a terminator or the next pairlist node");
    }

    // Tail recursion should be optimized out.
    recursive_parse(src, output, next_header, shared);
}

}

template<class Source_>
std::unique_ptr<PairList> parse_pairlist_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    auto output = std::make_unique<PairList>();
    pairlist_internal::recursive_parse(src, *output, header, shared);
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse a pairlist body", e);
    return std::unique_ptr<PairList>();
}

}

#endif
