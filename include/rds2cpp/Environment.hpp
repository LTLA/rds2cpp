#ifndef RDS2CPP_ENVIRONMENT_HPP
#define RDS2CPP_ENVIRONMENT_HPP

#include <vector>
#include <string>
#include "StringEncoding.hpp"
#include "SEXPType.hpp"

/**
 * @file Environment.hpp
 *
 * @brief Representation of an R environment.
 */

namespace rds2cpp {

/**
 * @brief An R environment.
 */
struct Environment {
    /**
     * Whether the environment was locked.
     */
    bool locked = false;

    /**
     * Type of the parent environment.
     */
    SEXPType parent_type = SEXPType::ENV;

    /** 
     * Index of the parent environment.
     * This should only be used if `paret_type` is `ENV`.
     */
    size_t parent = -1;

    /**
     * Names of the variables inside this environment.
     */
    std::vector<std::string> variable_names;

    /**
     * Encodings of the variable names in `variable_names`.
     */
    std::vector<StringEncoding> variable_encodings;

    /**
     * Values of the variables in this environment.
     */
    std::vector<std::unique_ptr<RObject> > variable_values;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

}

#endif

