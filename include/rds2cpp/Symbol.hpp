#ifndef RDS2CPP_SYMBOL_HPP
#define RDS2CPP_SYMBOL_HPP

#include <string>
#include "StringEncoding.hpp"

/**
 * @file Symbol.hpp
 *
 * @brief Representation of an R symbol.
 */

namespace rds2cpp {

/**
 * @brief An R symbol.
 */
struct Symbol {
    /**
     * Name of the symbol.
     */
    std::string name;

    /**
     * Encoding for the symbol name.
     */
    StringEncoding encoding;
};

}

#endif
