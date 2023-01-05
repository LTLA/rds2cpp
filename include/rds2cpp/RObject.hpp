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
    /**
     * @param i Value of the symbol index.
     */
    SymbolIndex(size_t i = -1) : index(i) {}


    SEXPType type() const { return SEXPType::SYM; }

    /**
     * Index into the `symbols` vector of the `RdsFile` object.
     */
    size_t index;
};

/**
 * @brief Reference to an environment. 
 */
struct EnvironmentIndex : public RObject {
    /**
     * By default, this creates an `EnvironmentIndex` that refers to the global environment.
     *
     * @param e Type of the environment.
     */
    EnvironmentIndex(SEXPType e = SEXPType::GLOBALENV_) : index(-1), env_type(e) {}

    /**
     * This creates an `EnvironmentIndex` that refers to a non-global environment, i.e., is of type `ENV`.
     *
     * @param i Value of the environment index. 
     */
    EnvironmentIndex(size_t i): index(i), env_type(SEXPType::ENV) {}

    /**
     * Type of environment.
     * Most environments will be `ENV` but special environments can be constructed like `GLOBALENV_`.
     */
    SEXPType type() const { return env_type; }

    /**
     * Index into the `environments` vector of the `RdsFile` object.
     * This is only used if `type()` returns `ENV`.
     */
    size_t index;

    /**
     * Type of the environment, as returned by `type()`.
     * This can be modified if it changes after construction. 
     */
    SEXPType env_type;
};

/**
 * @brief Reference to an external pointer.
 */
struct ExternalPointerIndex : public RObject {
    /**
     * @param i Value of the external pointer index.
     */
    ExternalPointerIndex(size_t i = -1) : index(i) {}


    SEXPType type() const { return SEXPType::EXTPTR; }

    /**
     * Index into the `external_pointers` vector of the `RdsFile` object.
     */
    size_t index;
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

    /**
     * A convenient helper to add an attribute.
     *
     * @param n Name of the attribute.
     * @param v Pointer to the attribute value.
     * This should not be owned by any other object.
     * @param enc Encoding of the attribute name.
     */
    void add(std::string n, RObject* v, StringEncoding enc = StringEncoding::UTF8) {
        names.emplace_back(std::move(n));
        encodings.emplace_back(enc);
        values.emplace_back(v);
    }

    /**
     * A convenient helper to add an attribute.
     *
     * @param n Name of the attribute.
     * @param v Unique pointer to the attribute value.
     * @param enc Encoding of the attribute name.
     */
    void add(std::string n, std::unique_ptr<RObject> v, StringEncoding enc = StringEncoding::UTF8) {
        names.emplace_back(std::move(n));
        encodings.emplace_back(enc);
        values.emplace_back(std::move(v));
    }
};

/**
 * @brief Vector of some atomic type.
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
 * @brief String vector.
 */
struct StringVector : public RObject {
    /**
     * @cond
     */
    StringVector(size_t n = 0) : data(n), encodings(n), missing(n) {}
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
     * A convenient helper to add a string to the end of the vector.
     *
     * @param d Value of the string.
     * @param enc Encoding of the attribute name.
     */
    void add(std::string d, StringEncoding enc = StringEncoding::UTF8) {
        data.push_back(std::move(d));
        encodings.push_back(enc);
        missing.push_back(false);
    }

    /**
     * A convenient helper to add a missing string to the end of the vector.
     */
    void add() {
        data.push_back("");
        encodings.push_back(StringEncoding::NONE);
        missing.push_back(true);
    }

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief Generic vector, i.e., an ordinary R list.
 */
struct GenericVector : public RObject {
    /**
     * @cond
     */
    GenericVector(size_t n = 0) : data(n) {}
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
     * A convenient helper to add a tagged element to the end of the pairlist.
     *
     * @param t Tag name.
     * @param d Unique pointer to the element value.
     * @param enc Encoding of the tag name.
     */
    void add(std::string t, std::unique_ptr<RObject> d, StringEncoding enc = StringEncoding::UTF8) {
        data.push_back(std::move(d));
        has_tag.push_back(true);
        tag_names.push_back(std::move(t));
        tag_encodings.push_back(enc);
    }

    /**
     * A convenient helper to add a tagged element to the end of the pairlist.
     *
     * @param t Tag name.
     * @param d Pointer to the element value.
     * This should not be owned by any other resource.
     * @param enc Encoding of the tag name.
     */
    void add(std::string t, RObject* d, StringEncoding enc = StringEncoding::UTF8) {
        data.emplace_back(d);
        has_tag.push_back(true);
        tag_names.push_back(std::move(t));
        tag_encodings.push_back(enc);
    }

    /**
     * A convenient helper to add an untagged element to the end of the pairlist.
     *
     * @param d Pointer to the element value.
     */
    void add(std::unique_ptr<RObject> d) {
        data.push_back(std::move(d));
        has_tag.push_back(false);
        tag_names.push_back("");
        tag_encodings.push_back(StringEncoding::NONE);
    }

    /**
     * A convenient helper to add an untagged element to the end of the pairlist.
     *
     * @param d Pointer to the element value.
     * This should not be owned by any other resource.
     */
    void add(RObject* d) {
        data.emplace_back(d);
        has_tag.push_back(false);
        tag_names.push_back("");
        tag_encodings.push_back(StringEncoding::NONE);
    }

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
    StringEncoding class_encoding = StringEncoding::UTF8;

    /**
     * Name of the package.
     */
    std::string package_name;

    /**
     * Encoding of the package name.
     */
    StringEncoding package_encoding = StringEncoding::UTF8;

    /**
     * Additional attributes.
     * For S4 objects, this is where the values of all slots are stored.
     */
    Attributes attributes;
};

/**
 * @brief Built-in function.
 */
struct BuiltInFunction : public RObject {
    SEXPType type() const { return SEXPType::BUILTIN; }

    /**
     * Name of the built-in function.
     */
    std::string name;
};

/**
 * @brief Language object, i.e., a function call.
 */
struct LanguageObject : public RObject {

    SEXPType type() const { return SEXPType::LANG; }

    /**
     * Name of the function.
     */
    std::string function_name;

    /**
     * Encoding for the function name.
     */
    StringEncoding function_encoding = StringEncoding::UTF8;

    /**
     * Values of the arguments to the function, typically `LanguageObject` or `SymbolIndex` objects.
     * These may also be `AtomicVector` instances of length 1.
     */
    std::vector<std::unique_ptr<RObject> > argument_values;

    /**
     * Whether or not each argument is named.
     * This should have the same length as `argument_values`.
     */
    std::vector<std::string> argument_names;

    /**
     * The name of the argument, if the corresponding entry of `argument_names` is `true`.
     * This should have the same length as `argument_values`.
     */
    std::vector<unsigned char> argument_has_name;

    /**
     * Encoding of the argument name, if the corresponding entry of `argument_names` is `true`.
     * This should have the same length as `argument_values`.
     */
    std::vector<StringEncoding> argument_encodings;

    /**
     * A convenient helper to add a named argument to the end of the argument list.
     *
     * @param n Argument name.
     * @param d Unique pointer to the argument value.
     * This should not be owned by any other resource.
     * @param enc Encoding of the argument name.
     */
    void add_argument(std::string n, RObject* d, StringEncoding enc = StringEncoding::UTF8) {
        argument_names.push_back(std::move(n));
        argument_values.emplace_back(d);
        argument_has_name.push_back(true);
        argument_encodings.push_back(enc);
    }

    /**
     * A convenient helper to add a named argument to the end of the argument list.
     *
     * @param n Argument name.
     * @param d Unique pointer to the argument value.
     * @param enc Encoding of the argument name.
     */
    void add_argument(std::string n, std::unique_ptr<RObject> d, StringEncoding enc = StringEncoding::UTF8) {
        argument_names.push_back(std::move(n));
        argument_values.push_back(std::move(d));
        argument_has_name.push_back(true);
        argument_encodings.push_back(enc);
    }

    /**
     * A convenient helper to add an unnamed argument to the end of the argument list.
     *
     * @param d Pointer to the argument value.
     * This should not be owned by any other resource.
     */
    void add_argument(RObject* d) {
        argument_names.resize(argument_names.size() + 1);
        argument_values.emplace_back(d);
        argument_has_name.push_back(false);
        argument_encodings.push_back(StringEncoding::NONE);
    }

    /**
     * A convenient helper to add an unnamed argument to the end of the argument list.
     *
     * @param d Pointer to the argument value.
     */
    void add_argument(std::unique_ptr<RObject> d) {
        argument_names.resize(argument_names.size() + 1);
        argument_values.push_back(std::move(d));
        argument_has_name.push_back(false);
        argument_encodings.push_back(StringEncoding::NONE);
    }

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief Expression vector.
 */
struct ExpressionVector : public RObject {
    /**
     * @cond
     */
    ExpressionVector(size_t n = 0) : data(n) {}
    /**
     * @endcond
     */

    SEXPType type() const { return SEXPType::EXPR; }

    /**
     * Vector of pointers to R expressions, typically `LanguageObject` or `SymbolIndex` objects.
     * These may also be `AtomicVector` instances of length 1.
     */
    std::vector<std::unique_ptr<RObject> > data;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

}

#endif
