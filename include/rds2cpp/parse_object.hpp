#ifndef RDS2CPP_PARSE_OBJECT_HPP
#define RDS2CPP_PARSE_OBJECT_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>

#include "SEXPType.hpp"
#include "RObject.hpp"
#include "utils.hpp"

#include "parse_atomic.hpp"
#include "parse_list.hpp"
#include "parse_attributes.hpp"
#include "parse_pairlist.hpp"
#include "parse_symbol.hpp"
#include "parse_altrep.hpp"

namespace rds2cpp {

template<class Reader>
std::shared_ptr<RObject> parse_object(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto details = parse_header(reader, leftovers);
    int sexp_type = details[3];
    std::shared_ptr<RObject> output;

    if (sexp_type == LIST) {
        output.reset(parse_pairlist(reader, leftovers, details));

    } else if (sexp_type == SYM) {
        output.reset(parse_symbol(reader, leftovers));

    } else if (sexp_type == 238) {
        output.reset(parse_altrep(reader, leftovers));

    } else if (sexp_type == 255) {
        output.reset(new Null);

    } else {
        if (sexp_type == INT) {
            output.reset(parse_integer(reader, leftovers));
        } else if (sexp_type == LGL) { 
            output.reset(parse_logical(reader, leftovers));
        } else if (sexp_type == RAW) {
            output.reset(parse_raw(reader, leftovers));
        } else if (sexp_type == REAL) {
            output.reset(parse_double(reader, leftovers));
        } else if (sexp_type == CPLX) {
            output.reset(parse_complex(reader, leftovers));
        } else if (sexp_type == STR) {
            output.reset(parse_character(reader, leftovers));
        } else if (sexp_type == VEC) {
            output.reset(parse_list(reader, leftovers));
        }

        if (has_attributes(details)) {
            parse_attributes(reader, leftovers, *output);
        }
    }

    return output;
}

}

#endif
