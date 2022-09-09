#ifndef RDS2CPP_PARSE_LIST_HPP
#define RDS2CPP_PARSE_LIST_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils.hpp"

namespace rds2cpp {

template<class Reader>
std::shared_ptr<RObject> parse_object(Reader&, std::vector<unsigned char>&);

template<class Reader>
List* parse_list(Reader& reader, std::vector<unsigned char>& leftovers) {
    size_t len = get_length(reader, leftovers);
    List output(len);
    for (size_t i = 0; i < len; ++i) {
        output.data[i] = parse_object(reader, leftovers);        
    }
    return new List(std::move(output));
}

}

#endif
