#ifndef RDS2CPP_PARSE_LIST_HPP
#define RDS2CPP_PARSE_LIST_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"
#include "SharedParseInfo.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&, SharedParseInfo& shared);

template<class Reader>
GenericVector parse_list_body(Reader& reader, std::vector<unsigned char>& leftovers, SharedParseInfo& shared) try {
    size_t len = get_length(reader, leftovers);
    GenericVector output(len);
    for (size_t i = 0; i < len; ++i) {
        try {
            output.data[i] = parse_object(reader, leftovers, shared);
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
