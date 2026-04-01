#ifndef RDS2CPP_PARSE_EXPRESSION_HPP
#define RDS2CPP_PARSE_EXPRESSION_HPP

#include <memory>
#include <vector>
#include <stdexcept>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo& shared);

template<class Source_>
std::unique_ptr<ExpressionVector> parse_expression_body(Source_& src, SharedParseInfo& shared) try {
    const auto len = get_length(src);
    auto output = std::make_unique<ExpressionVector>(len);
    for (I<decltype(len)> i = 0; i < len; ++i) {
        output->data[i] = parse_object(src, shared);
    }
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an expression body", e);
    return std::unique_ptr<ExpressionVector>();
}

}

#endif
