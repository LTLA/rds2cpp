#ifndef RDS2CPP_PARSE_PAIRLIST_HPP
#define RDS2CPP_PARSE_PAIRLIST_HPP

#include <cstdint>
#include <vector>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Reader>
std::shared_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&);

template<class Reader>
void parse_attributes(Reader&, std::vector<unsigned char>&, RObject*);

template<class Reader>
void parse_pairlist_internal(Reader& reader, std::vector<unsigned char>& leftovers, PairList& output, const Header& header) {
    bool has_attr = header[2] & 0x2;
    bool has_tag = header[2] & 0x4;

    if (has_attr) {
        parse_attributes(reader, leftovers, output);
    }

    if (has_tag) {
        auto header = parse_header(reader, leftovers);
        if (header[3] != 1) {
            throw std::runtime_error("expected a SYMSXP for a pairlist tag");
        }
        output.tags.emplace_back(true, parse_single_string(reader, leftovers));
    } else {
        output.tags.emplace_back(false, String());
    }

    output.data.push_back(parse_object(reader, leftovers));

    auto next_header = parse_header(reader, leftovers);
    if (next_header[3] == 254) {
        return;
    } else if (next_header[3] != 2) {
        throw std::runtime_error("expected a terminator or the next pairlist node");
    }

    parse_pairlist_internal(reader, leftovers, output, next_header);
    return;
}

template<class Reader>
PairList* parse_pairlist(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header) {
    PairList output;
    parse_pairlist_internal(reader, leftovers, output, header);
    return new PairList(std::move(output));
}

}

#endif
