#ifndef RDS2CPP_PARSE_ALTREP_HPP
#define RDS2CPP_PARSE_ALTREP_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_single_string.hpp"
#include "parse_atomic.hpp"

namespace rds2cpp {

namespace altrep_internal {

template<class Vector, class Reader>
Vector* parse_numeric_compact_seq(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto header = parse_header(reader, leftovers);
    if (header[3] != REAL) {
        throw std::runtime_error("expected compact_seq to store sequence information in doubles");
    }

    std::shared_ptr<DoubleVector> info(parse_double(reader, leftovers));
    const auto& ranges = info->data;
    if (ranges.size() != 3) {
        throw std::runtime_error("expected compact_seq's sequence information to be of length 3");
    }

    size_t len = ranges[0];
    double start = ranges[1], step = ranges[2];

    Vector output(len);
    for (size_t i = 0; i < len; ++i, start += step) {
        output.data[i] = start;
    }

    auto terminator = parse_header(reader, leftovers);
    if (terminator[3] != 254) {
        throw std::runtime_error("failed to terminate a compact_seq ALTREP correctly");
    }

    return new Vector(std::move(output));
}

template<class Vector, class Reader>
Vector* parse_attribute_wrapper(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto plist_header = parse_header(reader, leftovers);
    if (plist_header[3] != LIST) {
        throw std::runtime_error("expected pairlist in wrapper ALTREP's payload");
    }

    // First pairlist element is a CONS cell where the first value is the wrapped integer vector.
    auto contents = parse_object(reader, leftovers);
    if (contents->sexp_type != Vector::vector_sexp_type) {
        throw std::runtime_error("incorrectly typed contents in wrapper ALTREP's payload");
    }

    // Second cons value is the wrapping metadata, we don't care about it.
    auto metaheader = parse_header(reader, leftovers);
    if (metaheader[3] != INT) {
        throw std::runtime_error("wrap_* ALTREP should have an integer vector for its metadata");
    }
    std::unique_ptr<IntegerVector> metadata(parse_integer(reader, leftovers));
    if (metadata->data.size() != 2) {
        throw std::runtime_error("wrap_* ALTREP's metadata should be a length-2 integer vector");
    }

    // Now we can finally get the attributes, which makes up the rest of the pairlist.
    parse_attributes(reader, leftovers, *contents);

    auto coerced = static_cast<Vector*>(contents.get());
    return new Vector(std::move(*coerced));
}

}

template<class Reader>
RObject* parse_altrep(Reader& reader, std::vector<unsigned char>& leftovers) {
    std::unique_ptr<PairList> plist(parse_pairlist(reader, leftovers));
    if (plist->data.size() < 1 || plist->data[0]->sexp_type != SYM) {
        throw std::runtime_error("expected type specification symbol in the ALTREP description");
    }
    
    auto symb = static_cast<Symbol*>(plist->data[0].get());
    if (symb->name == "wrap_integer") {
        return altrep_internal::parse_attribute_wrapper<IntegerVector>(reader, leftovers);
    } else if (symb->name == "compact_intseq") {
        return altrep_internal::parse_numeric_compact_seq<IntegerVector>(reader, leftovers);
    } else {
        throw std::runtime_error("unrecognized ALTREP type '" + symb->name + "'");
    }

    return nullptr;
}

}

#endif
