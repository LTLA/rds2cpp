#ifndef RDS2CPP_PARSE_LIST_HPP
#define RDS2CPP_PARSE_LIST_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_&, SharedParseInfo& shared);

template<class Source_>
GenericVector parse_list_body(Source_& src, SharedParseInfo& shared) try {
    size_t len = get_length(src);
    GenericVector output(len);
    for (size_t i = 0; i < len; ++i) {
        try {
            output.data[i] = parse_object(src, shared);
        } catch (std::exception& e) {
            throw traceback("failed to parse list element " + std::to_string(i + 1), e);
        }
    }
    return output;
} catch (std::exception& e) {
    throw traceback("failed to parse an R list body", e);
}

}

#endif
