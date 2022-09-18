#ifndef RDS2CPP_PARSE_LIST_HPP
#define RDS2CPP_PARSE_LIST_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils.hpp"
#include "Shared.hpp"

namespace rds2cpp {

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&, Shared& shared);

template<class Reader>
GenericVector parse_list_body(Reader& reader, std::vector<unsigned char>& leftovers, Shared& shared) {
    size_t len = get_length(reader, leftovers);
    GenericVector output(len);
    for (size_t i = 0; i < len; ++i) {
        output.data[i] = parse_object(reader, leftovers, shared);
    }
    return output;
}

}

#endif
