#ifndef RDS2CPP_PARSE_SYMBOL_HPP
#define RDS2CPP_PARSE_SYMBOL_HPP

#include <stdexcept>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<SymbolIndex> parse_symbol_body(Reader& reader, SharedParseInfo& shared) try {
    auto str = parse_single_string(reader);
    if (!str.value.has_value()) {
        throw new std::runtime_error("expected a non-missing string for a symbol");
    }

    // Don't try to create a lvalue reference to 'shared.symbols[sindex]',
    // as this could be invalidated upon potential reallocations to 'shared.symbols' in the future.
    const auto sindex = request_new_symbol(shared);
    shared.symbols[sindex].name = std::move(*(str.value));
    shared.symbols[sindex].encoding = str.encoding;

    return std::make_unique<SymbolIndex>(sindex);
} catch (std::exception& e) {
    throw traceback("failed to parse a symbol body", e);
    return std::unique_ptr<SymbolIndex>();
}

}

#endif
