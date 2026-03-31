#ifndef RDS2CPP_RDSFILE_HPP
#define RDS2CPP_RDSFILE_HPP

#include <array>
#include <cstdint>

#include "RObject.hpp"
#include "Version.hpp"

/**
 * @file RdsFile.hpp
 *
 * @brief Information about an RDS file.
 */

namespace rds2cpp {

/**
 * @brief Contents of the parsed RDS file.
 */
struct RdsFile {
    /**
     * Version of the R serialization format.
     */
    std::int32_t format_version = 3;

    /**
     * R version used to write the file. 
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
     * This can be referenced by position using `EnvironmentIndex::index`.
     */
    std::vector<Environment> environments;

    /**
     * All symbols inside the file.
     * This can be referenced by position using `SymbolIndex::index`.
     */
    std::vector<Symbol> symbols;

    /**
     * All external pointers inside the file.
     * This can be referenced by position using `ExternalPointerIndex::index`.
     */
    std::vector<ExternalPointer> external_pointers;
};

}

#endif
