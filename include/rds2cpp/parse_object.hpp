#ifndef RDS2CPP_PARSE_OBJECT_HPP
#define RDS2CPP_PARSE_OBJECT_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_atomic.hpp"
#include "parse_list.hpp"
#include "SEXPType.hpp"

namespace rds2cpp {

template<class Reader>
std::shared_ptr<RObject> parse_object(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto details = parse_header(reader, leftovers);
    int sexp_type = details[3];
    std::shared_ptr<RObject> output;

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

    return output;
}

}

#endif
