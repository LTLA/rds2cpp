#ifndef RDS2CPP_PARSE_SYMBOL_HPP
#define RDS2CPP_PARSE_SYMBOL_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Reader>
SymbolIndex parse_symbol_body(Reader& reader, std::vector<unsigned char>& leftovers, SharedParseInfo& shared) {
    auto str = parse_single_string(reader, leftovers);
    if (str.missing) {
        throw new std::runtime_error("expected a non-missing string for a symbol");
    }

    Symbol new_symb;
    new_symb.name = str.value;
    new_symb.encoding = str.encoding;

    return SymbolIndex(shared.add_symbol(std::move(new_symb)));
}

}

#endif
