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
    // Reading the object's header.
    std::array<unsigned char, 4> details;
    bool ok = extract_up_to(reader, leftovers, 4,
        [&](const unsigned char* buffer, size_t n, size_t i) -> void {
            for (size_t b = 0; b < n; ++b, ++i) {
                details[i] = buffer[b];
            }
        }
    );
    if (!ok) {
        throw std::runtime_error("missing or incomplete object header");
    }

    // Figuring out what to do about this.
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
