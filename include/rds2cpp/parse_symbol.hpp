#ifndef RDS2CPP_PARSE_SYMBOL_HPP
#define RDS2CPP_PARSE_SYMBOL_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "Shared.hpp"
#include "utils.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Reader>
SymbolIndex parse_symbol_body(Reader& reader, std::vector<unsigned char>& leftovers, Shared& shared) {
    auto str = parse_single_string(reader, leftovers);

    Symbol new_symb;
    new_symb.name = str.value;
    new_symb.encoding = str.encoding;

    SymbolIndex output;
    output.index = shared.add_symbol(std::move(new_symb));
    return output;
}

}

#endif
