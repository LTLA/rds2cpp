#ifndef RDS2CPP_PARSE_OBJECT_HPP
#define RDS2CPP_PARSE_OBJECT_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>

#include "SEXPType.hpp"
#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"

#include "parse_atomic.hpp"
#include "parse_list.hpp"
#include "parse_attributes.hpp"
#include "parse_pairlist.hpp"
#include "parse_symbol.hpp"
#include "parse_altrep.hpp"
#include "parse_s4.hpp"
#include "parse_environment.hpp"
#include "parse_external_pointer.hpp"
#include "parse_builtin.hpp"
#include "parse_language.hpp"
#include "parse_expression.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_& src, SharedParseInfo& shared) {
    auto details = parse_header(src);
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
        pointerize_(parse_pairlist_body(src, details, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::SYM)) {
        pointerize_(parse_symbol_body(src, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::S4)) {
        pointerize_(parse_s4_body(src, details, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::ALTREP_)) {
        output = parse_altrep_body(src, shared);

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::NIL) || sexp_type == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        pointerize_(Null());

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::ENV)) {
        pointerize_(parse_new_environment_body(src, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::EXTPTR)) {
        pointerize_(parse_external_pointer_body(src, details, shared));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::GLOBALENV_)) {
        pointerize_(parse_global_environment_body());

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::BASEENV_)) {
        pointerize_(parse_base_environment_body());

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::EMPTYENV_)) {
        pointerize_(parse_empty_environment_body());

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::REF)) {
        output = shared.resolve_reference(details);

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::BUILTIN)) {
        pointerize_(parse_builtin_body(src));

    } else if (sexp_type == static_cast<unsigned char>(SEXPType::LANG)) {
        pointerize_(parse_language_body(src, details, shared));

    } else {
        Attributes* attr = nullptr;

        if (sexp_type == static_cast<unsigned char>(SEXPType::INT)) {
            attr = pointerize_attr(parse_integer_body(src));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::LGL)) { 
            attr = pointerize_attr(parse_logical_body(src));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::RAW)) {
            attr = pointerize_attr(parse_raw_body(src));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::REAL)) {
            attr = pointerize_attr(parse_double_body(src));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::CPLX)) {
            attr = pointerize_attr(parse_complex_body(src));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::STR)) {
            attr = pointerize_attr(parse_string_body(src));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::VEC)) {
            attr = pointerize_attr(parse_list_body(src, shared));
        } else if (sexp_type == static_cast<unsigned char>(SEXPType::EXPR)) {
            attr = pointerize_attr(parse_expression_body(src, shared));
        } else {
            throw std::runtime_error("cannot read unknown (or unsupported) SEXP type " + std::to_string(static_cast<int>(sexp_type)));
        }

        if (has_attributes(details) && attr) {
            parse_attributes(src, *attr, shared);
        }
    }

    return output;
}

}

#endif
