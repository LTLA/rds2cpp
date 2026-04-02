#ifndef RDS2CPP_PARSE_BUILTIN_HPP
#define RDS2CPP_PARSE_BUILTIN_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<BuiltInFunction> parse_builtin_body(Source_& src) try {
    const auto len = get_length(src);
    auto output = std::make_unique<BuiltInFunction>();
    sanisizer::resize(output->name, len);
    quick_extract(src, len, reinterpret_cast<unsigned char*>(output->name.data()));
    return output;
} catch(std::exception& e) {
    throw traceback("failed to parse built-in function body", e);
    return std::unique_ptr<BuiltInFunction>();
}

}

#endif

