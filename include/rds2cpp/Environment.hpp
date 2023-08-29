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
     * Whether the environment was hashed.
     */
    bool hashed = false;

    /**
     * Type of the parent environment.
     * This is usually one of `SEXPType::ENV`, `SEXPType::GLOBALENV_`, `SEXPType::BASEENV_` or `SEXPType::EMPTYENV_`.
     */
    SEXPType parent_type = SEXPType::GLOBALENV_;

    /** 
     * Index of the parent environment.
     * This should only be used if `parent_type` is `SEXPType::ENV`.
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
     * A convenient helper to add a variable.
     *
     * @param n Name of the variable.
     * @param v Pointer to the variable value.
     * This should not be owned by any other object.
     * @param enc Encoding of the variable name.
     */
    void add(std::string n, RObject* v, StringEncoding enc = StringEncoding::UTF8) {
        variable_names.push_back(std::move(n));
        variable_values.emplace_back(v);
        variable_encodings.push_back(enc);
    }

    /**
     * A convenient helper to add a variable.
     *
     * @param n Name of the variable.
     * @param v Unique pointer to the variable value.
     * @param enc Encoding of the variable name.
     */
    void add(std::string n, std::unique_ptr<RObject> v, StringEncoding enc = StringEncoding::UTF8) {
        variable_names.push_back(std::move(n));
        variable_values.push_back(std::move(v));
        variable_encodings.push_back(enc);
    }

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

}

#endif

