#ifndef RDS2CPP_PARSE_SYMBOL_HPP
#define RDS2CPP_PARSE_SYMBOL_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"
#include "parse_single_string.hpp"

namespace rds2cpp {

template<class Reader>
Symbol* parse_symbol(Reader& reader, std::vector<unsigned char>& leftovers) {
    auto str = parse_single_string(reader, leftovers);
    Symbol output;
    output.name = str.value;
    output.encoding = str.encoding;
    return new Symbol(std::move(output));
}

}

#endif
