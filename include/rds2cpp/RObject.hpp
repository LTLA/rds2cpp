#ifndef RDS2CPP_ROBJECT_HPP
#define RDS2CPP_ROBJECT_HPP

#include <vector>
#include <cstdint>
#include <complex>

#include "SEXPType.hpp"
#include "StringEncoding.hpp"

/**
 * @file RObject.hpp
 *
 * @brief Representations of unserialized R objects.
 */

namespace rds2cpp {

/**
 * @brief Virtual class for all unserialized R objects.
 */
struct RObject {
    /**
     * @cond
     */
    RObject() {}
    virtual ~RObject() {}

    // Rule of 5'ing.
    RObject& operator=(const RObject& rhs) = default;
    RObject(const RObject& rhs) = default;
    RObject& operator=(RObject&& rhs) = default;
    RObject(RObject&& rhs) = default;
    /**
     * @endcond
     */

    /**
     * @return Type of the R object.
     */
    virtual SEXPType type() const = 0;
};

/**
 * @brief R's `NULL` value.
 */
struct Null : public RObject {
    SEXPType type() const { return SEXPType::NIL; }
};

/**
 * @brief Reference to a language symbol. 
 */
struct SymbolIndex : public RObject {
    SEXPType type() const { return SEXPType::SYM; }

    /**
     * Index into the `symbols` vector of the `Parsed` object.
     */
    size_t index;
};

/**
 * @brief Reference to an environment. 
 */
struct EnvironmentIndex : public RObject {
    /**
     * Type of environment.
     * Most environments will be `ENV` but special environments may occur like `GLOBALENV_` and `BASEENV_`.
     */
    SEXPType type() const { return env_type; }

    /**
     * Index into the `environments` vector of the `Parsed` object.
     */
    size_t index;

    /**
     * @cond
     */
    SEXPType env_type = SEXPType::ENV;
    /**
     * @cond
     */
};

/**
 * @brief Attribute names and values.
 */
struct Attributes {
    /**
     * Name of each attribute.
     */
    std::vector<std::string> names;

    /**
     * Encoding of each attribute's name.
     */
    std::vector<StringEncoding> encodings;

    /**
     * Value of each attribute.
     */
    std::vector<std::unique_ptr<RObject> > values;
};

/**
 * @brief Vector of an atomic type.
 *
 * @tparam ElementType Data type for each element.
 * @tparam stype `SEXPType` flag for the type. 
 */
template<typename ElementType, SEXPType stype>
struct AtomicVector : public RObject {
    /**
     * @cond
     */
    AtomicVector(size_t n = 0) : data(n) {}
    static constexpr SEXPType vector_sexp_type = stype;
    /**
     * @endcond
     */

    SEXPType type() const { return stype; }

    /**
     * Contents of the vector.
     */
    std::vector<ElementType> data;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief Integer vector.
 */
typedef AtomicVector<int32_t, SEXPType::INT> IntegerVector;

/**
 * @brief Logical vector.
 */
typedef AtomicVector<int32_t, SEXPType::LGL> LogicalVector;

/**
 * @brief Double-precision vector.
 */
typedef AtomicVector<double, SEXPType::REAL> DoubleVector;

/**
 * @brief Raw vector.
 */
typedef AtomicVector<unsigned char, SEXPType::RAW> RawVector;

/**
 * @brief Complex vector.
 */
typedef AtomicVector<std::complex<double>, SEXPType::CPLX> ComplexVector;

/**
 * @brief Character (i.e., string) vector.
 */
struct CharacterVector : public RObject {
    /**
     * @cond
     */
    CharacterVector(size_t n = 0) : data(n), encodings(n), missing(n) {}
    /**
     * @endcond
     */

    SEXPType type() const { return SEXPType::STR; }

    /**
     * Contents of the vector.
     */
    std::vector<std::string> data;

    /**
     * Encodings for each element in `data`.
     */
    std::vector<StringEncoding> encodings;

    /**
     * Whether a string is missing in the vector.
     * If `true`, the corresponding value of `data` and `encoding` should not be used.
     */
    std::vector<char> missing;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief Generic vector, i.e., an ordinary list.
 */
struct List : public RObject {
    /**
     * @cond
     */
    List(size_t n = 0) : data(n) {}
    /**
     * @endcond
     */

    SEXPType type() const { return SEXPType::VEC; } 

    /**
     * Contents of the vector.
     */
    std::vector<std::unique_ptr<RObject> > data;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief Pairlist, i.e., a linked list.
 */
struct PairList : public RObject {
    SEXPType type() const { return SEXPType::LIST; }

    /**
     * Contents of the vector.
     */
    std::vector<std::unique_ptr<RObject> > data; 

    /**
     * Whether or not the corresponding element of `data` is tagged.
     * This is of length equal to `data`.
     */
    std::vector<unsigned char> has_tag;

    /**
     * Names of the tags.
     * Empty strings are used for untagged elements.
     */
    std::vector<std::string> tag_names;

    /**
     * Encoding of the tags.
     */
    std::vector<StringEncoding> tag_encodings;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief S4 object.
 */
struct S4Object : public RObject {
    SEXPType type() const { return SEXPType::S4; }

    /**
     * Name of the class.
     */
    std::string class_name;

    /**
     * Encoding of the class name.
     */
    StringEncoding class_encoding;

    /**
     * Name of the package.
     */
    std::string package_name;

    /**
     * Encoding of the package name.
     */
    StringEncoding package_encoding;

    /**
     * Additional attributes.
     * For S4 objects, this is where the values of all slots are stored.
     */
    Attributes attributes;
};

}

#endif
