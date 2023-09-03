#ifndef RDS2CPP_PARSE_EXPRESSION_HPP
#define RDS2CPP_PARSE_EXPRESSION_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo& shared);

template<class Source_>
ExpressionVector parse_expression_body(Source_& src, SharedParseInfo& shared) try {
    size_t len = get_length(src);
    ExpressionVector output(len);
    for (size_t i = 0; i < len; ++i) {
        output.data[i] = parse_object(src, shared);
    }
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an expression body", e);
}

}

#endif
