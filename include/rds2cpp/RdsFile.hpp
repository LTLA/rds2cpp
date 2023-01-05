#ifndef RDS2CPP_RDSFILE_HPP
#define RDS2CPP_RDSFILE_HPP

#include <array>
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
 * @brief Contents of the parsed RDS file.
 */
struct RdsFile {
    RdsFile() {
        format_version = 3;            

        writer_version[0] = 4;
        writer_version[1] = 2;
        writer_version[2] = 0;

        reader_version[0] = 3;
        reader_version[1] = 5;
        reader_version[2] = 0;

        encoding = "UTF-8";
    }

    /**
     * @cond
     */
    // Avoid any initialization.
    RdsFile(bool) {}
    /**
     * @endcond
     */

    /**
     * Version of the RDS format.
     */
    uint32_t format_version;

    /**
     * R version used to write the file as major-minor-patch integers.
     */
    std::array<unsigned char, 3> writer_version;

    /**
     * Minimum R version required to read the file as major-minor-patch integers.
     */
    std::array<unsigned char, 3> reader_version;

    /**
     * Encoding required to read the file.
     */
    std::string encoding;

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
