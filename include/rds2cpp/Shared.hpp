#ifndef RDS2CPP_SHARED_HPP
#define RDS2CPP_SHARED_HPP

#include <vector>
#include <memory>
#include <string>

#include "StringEncoding.hpp"
#include "RObject.hpp"

namespace rds2cpp {

struct Environment {
    size_t parent;
    std::vector<std::string> variable_names;
    std::vector<StringEncoding> variable_encodings;
    std::vector<std::unique_ptr<RObject> > variable_values;
};

struct Shared {
    std::vector<Environment> environments;    
    std::vector<size_t> environment_mappings;
};

}

#endif
