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
#include "parse_s4.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto details = parse_header(reader, leftovers);
    auto sexp_type = details[3];

    std::unique_ptr<RObject> output;
    auto pointerize_ = [&](auto obj) -> void {
        pointerize(output, std::move(obj));
        return;
    };

    if (sexp_type == static_cast<unsigned char>(SEXPType::LIST)) {
        pointerize_(parse_pairlist_body(reader, leftovers, details));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::SYM)) {
        pointerize_(parse_symbol_body(reader, leftovers));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::S4)) {
        pointerize_(parse_s4_body(reader, leftovers, details));

    } else if (sexp_type == 238) {
        output = parse_altrep_body(reader, leftovers);

    } else if (sexp_type == 254 || sexp_type == 255) {
        pointerize_(Null());

    } else {
        if (sexp_type == static_cast<unsigned char>(SEXPType::INT)) {
            pointerize_(parse_integer_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::LGL)) { 
            pointerize_(parse_logical_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::RAW)) {
            pointerize_(parse_raw_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::REAL)) {
            pointerize_(parse_double_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::CPLX)) {
            pointerize_(parse_complex_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::STR)) {
            pointerize_(parse_character_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::VEC)) {
            pointerize_(parse_list_body(reader, leftovers));
        }

        if (has_attributes(details)) {
            parse_attributes(reader, leftovers, *output);
        }
    }

    return output;
}

}

#endif
