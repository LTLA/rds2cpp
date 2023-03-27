#ifndef RDS2CPP_PARSE_BUILTIN_HPP
#define RDS2CPP_PARSE_BUILTIN_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

template<class Reader>
BuiltInFunction parse_builtin_body(Reader& reader, std::vector<unsigned char>& leftovers) try {
    size_t len = get_length(reader, leftovers);

    BuiltInFunction output;
    extract_up_to(reader, leftovers, len,
        [&](const unsigned char* buffer, size_t n, size_t) -> void {
            auto ptr = reinterpret_cast<const char*>(buffer);
            output.name.insert(output.name.end(), ptr, ptr + n);
        }
    );
    
    return output;
} catch(std::exception& e) {
    throw traceback("failed to parse built-in function body", e);
}

}

#endif

