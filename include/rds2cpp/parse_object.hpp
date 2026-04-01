#ifndef RDS2CPP_PARSE_OBJECT_HPP
#define RDS2CPP_PARSE_OBJECT_HPP

#include <memory>
#include <array>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

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

    auto pointerize_attr = [&](auto obj) -> void {
        if (has_attributes(details)) {
            parse_attributes(src, obj->attributes, shared);
        }
        output = std::move(obj);
    };

    switch(sexp_type) {
        case static_cast<unsigned char>(SEXPType::LIST):
            output = parse_pairlist_body(src, details, shared);
            break;

        case static_cast<unsigned char>(SEXPType::SYM):
            output = parse_symbol_body(src, shared);
            break;

        case static_cast<unsigned char>(SEXPType::S4):
            output = parse_s4_body(src, details, shared);
            break;

        case static_cast<unsigned char>(SEXPType::ALTREP_):
            output = parse_altrep_body(src, shared);
            break;

        case static_cast<unsigned char>(SEXPType::NIL):
        case static_cast<unsigned char>(SEXPType::NILVALUE_):
            output.reset(new Null);
            break;

        case static_cast<unsigned char>(SEXPType::ENV):
            output = parse_new_environment_body(src, shared);
            break;

        case static_cast<unsigned char>(SEXPType::EXTPTR):
            output = parse_external_pointer_body(src, details, shared);
            break;

        case static_cast<unsigned char>(SEXPType::GLOBALENV_):
            output = parse_global_environment_body();
            break;

        case static_cast<unsigned char>(SEXPType::BASEENV_):
            output = parse_base_environment_body();
            break;

        case static_cast<unsigned char>(SEXPType::EMPTYENV_):
            output = parse_empty_environment_body();
            break;

        case static_cast<unsigned char>(SEXPType::REF):
            output = resolve_reference(details, shared);
            break;

        case static_cast<unsigned char>(SEXPType::BUILTIN):
            output = parse_builtin_body(src);
            break;

        case static_cast<unsigned char>(SEXPType::LANG):
            output = parse_language_body(src, details, shared);
            break;

        case static_cast<unsigned char>(SEXPType::INT):
            pointerize_attr(parse_integer_body(src));
            break;

        case static_cast<unsigned char>(SEXPType::LGL):
            pointerize_attr(parse_logical_body(src));
            break;

        case static_cast<unsigned char>(SEXPType::RAW):
            pointerize_attr(parse_raw_body(src));
            break;

        case static_cast<unsigned char>(SEXPType::REAL):
            pointerize_attr(parse_double_body(src));
            break;

        case static_cast<unsigned char>(SEXPType::CPLX):
            pointerize_attr(parse_complex_body(src));
            break;

        case static_cast<unsigned char>(SEXPType::STR):
            pointerize_attr(parse_string_body(src));
            break;

        case static_cast<unsigned char>(SEXPType::VEC):
            pointerize_attr(parse_list_body(src, shared));
            break;

        case static_cast<unsigned char>(SEXPType::EXPR):
            pointerize_attr(parse_expression_body(src, shared));
            break;

        default:
            throw std::runtime_error("cannot read unknown (or unsupported) SEXP type " + std::to_string(static_cast<int>(sexp_type)));
    }

    return output;
}

}

#endif
