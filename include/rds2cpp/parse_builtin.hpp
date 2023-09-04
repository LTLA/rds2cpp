#ifndef RDS2CPP_PARSE_BUILTIN_HPP
#define RDS2CPP_PARSE_BUILTIN_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

template<class Source_>
BuiltInFunction parse_builtin_body(Source_& src) try {
    size_t len = get_length(src);

    BuiltInFunction output;
    output.name.resize(len);
    for (size_t i = 0; i < len; ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        output.name[i] = src.get();
    }

    return output;
} catch(std::exception& e) {
    throw traceback("failed to parse built-in function body", e);
}

}

#endif

