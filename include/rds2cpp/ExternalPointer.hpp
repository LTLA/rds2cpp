#ifndef RDS2CPP_EXTERNAL_POINTER_HPP
#define RDS2CPP_EXTERNAL_POINTER_HPP

#include <vector>
#include <string>
#include <memory>
#include "RObject.hpp"

/**
 * @file ExternalPointer.hpp
 *
 * @brief Representation of an external pointer.
 */

namespace rds2cpp {

/**
 * @brief An R external pointer.
 */
struct ExternalPointer {
    /**
     * Pointer to the external pointer's protection value.
     */
    std::unique_ptr<RObject> protection;

    /**
     * Pointer to the external pointer's tag.
     */
    std::unique_ptr<RObject> tag;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

}

#endif
