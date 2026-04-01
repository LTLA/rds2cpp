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
std::unique_ptr<PairList> parse_pairlist_body(Source_&, const Header&, SharedParseInfo&);

template<class Source_>
std::unique_ptr<LanguageObject> parse_language_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    auto output = std::make_unique<LanguageObject>();

    auto contents = parse_pairlist_body(src, header, shared);
    output->attributes = std::move(contents->attributes);

    const auto num_contents = contents->data.size();
    if (num_contents < 1) {
        throw std::runtime_error("pairlist should have positive length for language objects");
    }
    if (contents->data[0].value->type() != SEXPType::SYM) {
        throw std::runtime_error("first pairlist entry for a language object should be a symbol");
    }

    auto ptr = static_cast<const SymbolIndex*>(contents->data[0].value.get());
    output->function = SymbolIndex(ptr->index);

    // Shifting the rest into the arguments.
    output->arguments.reserve(num_contents - 1);
    for (I<decltype(num_contents)> i = 1; i < num_contents; ++i) {
        auto& current = contents->data[i];
        if (current.tag.has_value()) {
            output->arguments.emplace_back(std::move(*(current.tag)), std::move(current.value));
        } else {
            output->arguments.emplace_back(std::move(current.value));
        }
    }

    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an R language object's body", e);
    return std::unique_ptr<LanguageObject>();
}

}

#endif
