#ifndef RDS2CPP_RDAFILE_HPP
#define RDS2CPP_RDAFILE_HPP

#include <array>
#include <cstdint>

#include "RObject.hpp"
#include "Version.hpp"

/**
 * @file RdaFile.hpp
 *
 * @brief Information about an RDA file.
 */

namespace rds2cpp {

/**
 * @brief R object saved in a `RdaFile`.
 */
struct RdaObject {
    /**
     * Default constructor.
     */
    RdaObject() = default;

    /**
     * @param name Name of the object.
     * @param value Value of the object.
     */
    RdaObject(SymbolIndex name, std::unique_ptr<RObject> value) : name(std::move(name)), value(std::move(value)) {}

    /**
     * Name of the object.
     */
    SymbolIndex name;

    /**
     * Value of the object.
     */
    std::unique_ptr<RObject> value;
};

/**
 * @brief Contents of the parsed RDA file.
 */
struct RdaFile {
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
     * Vector of objects saved in this file along with their names.
     */
    std::vector<RdaObject> objects;

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
