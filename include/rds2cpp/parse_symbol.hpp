#ifndef RDS2CPP_PARSE_SYMBOL_HPP
#define RDS2CPP_PARSE_SYMBOL_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Reader>
SymbolIndex parse_symbol_body(Reader& reader, SharedParseInfo& shared) try {
    auto str = parse_single_string(reader);
    if (str.missing) {
        throw new std::runtime_error("expected a non-missing string for a symbol");
    }

    size_t idx = shared.request_symbol();
    shared.symbols[idx].name = str.value;
    shared.symbols[idx].encoding = str.encoding;

    return SymbolIndex(idx);
} catch (std::exception& e) {
    throw traceback("failed to parse a symbol body", e);
}

}

#endif
