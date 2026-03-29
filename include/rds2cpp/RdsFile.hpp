#ifndef RDS2CPP_RDSFILE_HPP
#define RDS2CPP_RDSFILE_HPP

#include <array>
#include <cstdint>

#include "RObject.hpp"
#include "Environment.hpp"
#include "ExternalPointer.hpp"
#include "Symbol.hpp"

/**
 * @file RdsFile.hpp
 *
 * @brief Information about an RDS file.
 */

namespace rds2cpp {

/**
 * @brief Major-minor-patch version.
 */
struct Version {
    /**
     * Major version.
     */
    std::int16_t major = 3;

    /**
     * Minor version.
     */
    std::uint8_t minor = 5;

    /**
     * Patch version.
     */
    std::uint8_t patch = 0;
};

/**
 * @brief Contents of the parsed RDS file.
 */
struct RdsFile {
    /**
     * Version of the RDS format.
     */
    std::int32_t format_version = 3;

    /**
     * R version used to write the file as major-minor-patch integers.
     */
    Version writer_version;

    /**
     * Minimum R version required to read the file.
     */
    Version reader_version;

    /**
     * String encoding used to write the file, for conversion of unflagged strings.
     */
    StringEncoding encoding = StringEncoding::UTF8;

    /**
     * The unserialized object.
     */
    std::unique_ptr<RObject> object;

    /**
     * All environments inside the file.
     * This can be referenced by the `index` in `EnvironmentIndex`.
     */
    std::vector<Environment> environments;

    /**
     * All symbols inside the file.
     * This can be referenced by the `index` in `SymbolIndex`.
     */
    std::vector<Symbol> symbols;

    /**
     * All external pointers inside the file.
     * This can be referenced by the `index` in `ExternalPointerIndex`.
     */
    std::vector<ExternalPointer> external_pointers;
};

}

#endif
