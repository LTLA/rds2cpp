#ifndef RDS2CPP_PARSE_LANGUAGE_HPP
#define RDS2CPP_PARSE_LANGUAGE_HPP

#include <vector>
#include <memory>
#include <stdexcept>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Source_>
PairList parse_pairlist_body(Source_&, const Header&, SharedParseInfo&);

template<class Source_>
LanguageObject parse_language_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    LanguageObject output;

    auto contents = parse_pairlist_body(src, header, shared);
    output.attributes = std::move(contents.attributes);

    if (contents.has_tag.size() < 1) {
        throw std::runtime_error("pairlist should have positive length for language objects");
    }
    if (contents.data[0]->type() != SEXPType::SYM) {
        throw std::runtime_error("first pairlist entry for a language object should be a symbol");
    }

    auto ptr = static_cast<const SymbolIndex*>(contents.data[0].get());
    const auto& symb = shared.symbols[ptr->index];
    output.function_name = symb.name;
    output.function_encoding = symb.encoding;

    // Shifting the rest into the arguments.
    contents.has_tag.erase(contents.has_tag.begin());
    output.argument_has_name = std::move(contents.has_tag);

    contents.tag_names.erase(contents.tag_names.begin());
    output.argument_names = std::move(contents.tag_names);

    contents.tag_encodings.erase(contents.tag_encodings.begin());
    output.argument_encodings = std::move(contents.tag_encodings);

    contents.data.erase(contents.data.begin());
    output.argument_values = std::move(contents.data);

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an R language object's body", e);
}

}

#endif
