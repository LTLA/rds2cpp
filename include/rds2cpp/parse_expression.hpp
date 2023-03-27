#ifndef RDS2CPP_PARSE_EXPRESSION_HPP
#define RDS2CPP_PARSE_EXPRESSION_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&, SharedParseInfo& shared);

template<class Reader>
ExpressionVector parse_expression_body(Reader& reader, std::vector<unsigned char>& leftovers, SharedParseInfo& shared) try {
    size_t len = get_length(reader, leftovers);
    ExpressionVector output(len);
    for (size_t i = 0; i < len; ++i) {
        output.data[i] = parse_object(reader, leftovers, shared);
    }
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an expression body", e);
}

}

#endif
