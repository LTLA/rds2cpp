#ifndef RDS2CPP_PARSE_OBJECT_HPP
#define RDS2CPP_PARSE_OBJECT_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>

#include "SEXPType.hpp"
#include "RObject.hpp"
#include "Shared.hpp"
#include "utils.hpp"

#include "parse_atomic.hpp"
#include "parse_list.hpp"
#include "parse_attributes.hpp"
#include "parse_pairlist.hpp"
#include "parse_symbol.hpp"
#include "parse_altrep.hpp"
#include "parse_s4.hpp"
#include "parse_environment.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader& reader, std::vector<unsigned char>& leftovers, Shared& shared) {
    auto details = parse_header(reader, leftovers);
    auto sexp_type = details[3];

    std::unique_ptr<RObject> output;
    auto pointerize_ = [&](auto obj) -> void {
        pointerize(output, std::move(obj));
        return;
    };

    auto pointerize_attr = [&](auto obj) -> Attributes* {
        pointerize(output, std::move(obj));
        typedef typename std::remove_reference<decltype(obj)>::type Object;
        auto ptr = static_cast<Object*>(output.get());
        return &(ptr->attributes);
    };

    if (sexp_type == static_cast<unsigned char>(SEXPType::LIST)) {
        pointerize_(parse_pairlist_body(reader, leftovers, details, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::SYM)) {
        pointerize_(parse_symbol_body(reader, leftovers, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::S4)) {
        pointerize_(parse_s4_body(reader, leftovers, details, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::ALTREP_)) {
        output = parse_altrep_body(reader, leftovers, shared);

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::NIL) || sexp_type == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        pointerize_(Null());

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::ENV)) {
        pointerize_(parse_new_environment_body(reader, leftovers, details, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::GLOBALENV_)) {
        pointerize_(parse_global_environment_body(reader, leftovers, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::REF)) {
        output = shared.resolve_reference(details);

    } else {
        Attributes* attr = nullptr;

        if (sexp_type == static_cast<unsigned char>(SEXPType::INT)) {
            attr = pointerize_attr(parse_integer_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::LGL)) { 
            attr = pointerize_attr(parse_logical_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::RAW)) {
            attr = pointerize_attr(parse_raw_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::REAL)) {
            attr = pointerize_attr(parse_double_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::CPLX)) {
            attr = pointerize_attr(parse_complex_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::STR)) {
            attr = pointerize_attr(parse_character_body(reader, leftovers));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::VEC)) {
            attr = pointerize_attr(parse_list_body(reader, leftovers, shared));
        }

        if (has_attributes(details) && attr) {
            parse_attributes(reader, leftovers, *attr, shared);
        }
    }

    return output;
}

}

#endif