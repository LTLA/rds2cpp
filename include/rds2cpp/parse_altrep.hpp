#ifndef RDS2CPP_PARSE_ALTREP_HPP
#define RDS2CPP_PARSE_ALTREP_HPP

#include <cstdint>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "parse_single_string.hpp"
#include "parse_attributes.hpp"

namespace rds2cpp {

template<class Source_>
IntegerVector parse_integer_body(Source_&);

template<class Source_>
DoubleVector parse_double_body(Source_& src);

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo&);

template<class Source_>
PairList parse_pairlist_body(Source_&, const Header&, SharedParseInfo&);

namespace altrep_internal {

template<class Vector, class Source_>
Vector parse_numeric_compact_seq(Source_& src) try {
    auto header = parse_header(src);
    if (header[3] != static_cast<unsigned char>(SEXPType::REAL)) {
        throw std::runtime_error("expected compact_seq to store sequence information in doubles");
    }

    auto info = parse_double_body(src);
    const auto& ranges = info.data;
    if (ranges.size() != 3) {
        throw std::runtime_error("expected compact_seq's sequence information to be of length 3");
    }

    size_t len = ranges[0];
    double start = ranges[1], step = ranges[2];

    Vector output(len);
    for (size_t i = 0; i < len; ++i, start += step) {
        output.data[i] = start;
    }

    auto terminator = parse_header(src);
    if (terminator[3] != 254) {
        throw std::runtime_error("failed to terminate a compact_seq ALTREP correctly");
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse compact numeric ALTREP", e);
}

template<class Vector, class Source_>
Vector parse_attribute_wrapper(Source_& src, SharedParseInfo& shared) try {
    auto plist_header = parse_header(src);
    if (plist_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected pairlist in wrap_* ALTREP's payload");
    }

    // First pairlist element is a CONS cell where the first value is the wrapped integer vector.

    auto contents = parse_object(src, shared);
    if (contents->type() != Vector::vector_sexp_type) {
        throw std::runtime_error("incorrectly typed contents in wrap_* ALTREP's payload");
    }

    // Second cons value is the wrapping metadata, we don't care about it.
    auto metaheader = parse_header(src);
    if (metaheader[3] != static_cast<unsigned char>(SEXPType::INT)) {
        throw std::runtime_error("wrap_* ALTREP should have an integer vector for its metadata");
    }

    auto metadata = parse_integer_body(src);
    if (metadata.data.size() != 2) {
        throw std::runtime_error("wrap_* ALTREP's metadata should be a length-2 integer vector");
    }

    // Now we can finally get the attributes, which makes up the rest of the pairlist.
    auto coerced = static_cast<Vector*>(contents.get());
    auto attrheader = parse_header(src);
    if (attrheader[3] == static_cast<unsigned>(SEXPType::LIST)) {
        parse_attributes_body(src, attrheader, coerced->attributes, shared);
    } else if (attrheader[3] != static_cast<unsigned>(SEXPType::NILVALUE_)) {
        throw std::runtime_error("wrap_* ALTREP's attributes should be a pairlist or NULL");
    }

    return Vector(std::move(*coerced));
} catch (std::exception& e) {
    throw traceback("failed to parse attribute-wrapped ALTREP", e);
}

template<class Source_>
StringVector parse_deferred_string(Source_& src, SharedParseInfo& shared) try {
    auto plist_header = parse_header(src);
    if (plist_header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected pairlist in deferred_string ALTREP's payload");
    }

    // First pairlist element is a CONS cell where the first value is the thing to be converted.
    auto contents = parse_object(src, shared);
    StringVector output;

    if (contents->type() == SEXPType::INT){
        auto cast = static_cast<IntegerVector*>(contents.get());
        size_t n = cast->data.size();
        output = StringVector(n);

        for (size_t i = 0; i < n; ++i) {
            if (cast->data[i] == std::numeric_limits<int32_t>::min()) { // see R_ext/Arith.h
                output.missing[i] = true;
            } else {
                output.data[i] = std::to_string(cast->data[i]);
                output.encodings[i] = StringEncoding::ASCII;
            }
        }

    } else if (contents->type() == SEXPType::REAL) {
        std::ostringstream converter;
        converter.precision(std::numeric_limits<double>::max_digits10);
        auto cast = static_cast<DoubleVector*>(contents.get());
        bool lw = (little_endian() ? 0 : 1); // see R_ext/Arith.h
        output = StringVector(cast->data.size());

        for (size_t i = 0; i < cast->data.size(); ++i) {
            output.encodings[i] = StringEncoding::ASCII;

            if (std::isfinite(cast->data[i])) {
                converter << cast->data[i];
                output.data[i] = converter.str();
                converter.str(std::string());

            } else if (std::isnan(cast->data[i])) {
                auto ptr = reinterpret_cast<uint32_t*>(&(cast->data[i]));
                if (ptr[lw] == 1954) { // see R_ext/Arith.h.
                    output.missing[i] = true;
                } else {
                    output.data[i] = "NaN";
                }

            } else if (std::isinf(cast->data[i])) {
                if (cast->data[i] > 0) {
                    output.data[i] = "Inf";
                } else {
                    output.data[i] = "-Inf";
                }
            }
        }

    } else {
        throw std::runtime_error("unsupported content type in deferred_string ALTREP's payload");
    }

    // Second cons value is the wrapping metadata, we don't care about it.
    auto metaheader = parse_header(src);
    if (metaheader[3] != static_cast<unsigned char>(SEXPType::INT)) {
        throw std::runtime_error("deferred_string ALTREP should have an integer vector for its metadata");
    }

    auto metadata = parse_integer_body(src);
    if (metadata.data.size() != 1) {
        throw std::runtime_error("deferred_string ALTREP's metadata should be a length-1 integer vector");
    }

    // Chomp up the null.
    auto terminator = parse_header(src);
    if (terminator[3] != static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        throw std::runtime_error("failed to terminate a deferred string ALTREP correctly");
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse deferred string ALTREP", e);
}

}

template<class Source_>
std::unique_ptr<RObject> parse_altrep_body(Source_& src, SharedParseInfo& shared) try {
    auto header = parse_header(src);
    if (header[3] != static_cast<unsigned char>(SEXPType::LIST)) {
        throw std::runtime_error("expected ALTREP description to be a pairlist");
    }

    auto plist = parse_pairlist_body(src, header, shared);
    if (plist.data.size() < 1 || plist.data[0]->type() != SEXPType::SYM) {
        throw std::runtime_error("expected type specification symbol in the ALTREP description");
    }

    std::unique_ptr<RObject> output;
    auto pointerize_ = [&](auto x) -> void {
        pointerize(output, std::move(x));
    };

    auto sdx = static_cast<SymbolIndex*>(plist.data[0].get());
    const auto& symb = shared.symbols[sdx->index];

    if (symb.name == "wrap_integer") {
        pointerize_(altrep_internal::parse_attribute_wrapper<IntegerVector>(src, shared));
    } else if (symb.name == "compact_intseq") {
        pointerize_(altrep_internal::parse_numeric_compact_seq<IntegerVector>(src));
    } else if (symb.name == "deferred_string") {
        pointerize_(altrep_internal::parse_deferred_string(src, shared));
    } else {
        throw std::runtime_error("unrecognized ALTREP type '" + symb.name + "'");
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse ALTREP body", e);
}

}

#endif
