#ifndef RDS2CPP_ROBJECT_HPP
#define RDS2CPP_ROBJECT_HPP

#include <vector>
#include <cstdint>
#include <cstddef>
#include <complex>
#include <optional>

#include "SEXPType.hpp"
#include "StringEncoding.hpp"
#include "utils_other.hpp"

#include "sanisizer/sanisizer.hpp"

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
struct Null final : public RObject {
    SEXPType type() const { return SEXPType::NIL; }
};

/**
 * @brief An R symbol.
 */
struct Symbol {
    /**
     * Default constructor.
     */
    Symbol() = default;

    /**
     * @param name Name of the symbol.
     * @param encoding Character encoding of the name.
     */
    Symbol(std::string name, StringEncoding encoding) : name(std::move(name)), encoding(encoding) {}

    /**
     * Name of the symbol.
     */
    std::string name;

    /**
     * Encoding for the symbol name.
     */
    StringEncoding encoding;
};

/**
 * @brief Reference to a language symbol. 
 */
struct SymbolIndex final : public RObject {
    /**
     * Default constructor.
     */
    SymbolIndex() = default;

    /**
     * @param i Value of the symbol index.
     */
    SymbolIndex(std::size_t i) : index(i) {}


    SEXPType type() const { return SEXPType::SYM; }

    /**
     * Index of the `RdsFile::symbols` or `RdaFile::symbols` vector, containing the value and encoding of the symbol.
     */
    std::size_t index = - 1;
};

/**
 * @param name Name of the symbol, see `Symbol::name`.
 * @param name Encoding of the symbol, see `Symbol::encoding`.
 * @param symbols Vector containing the global set of symbols, typically `RdsFile::symbols` or `RdaFile::symbols`.
 *
 * @return A new symbol is added to `symbols` and a `SymbolIndex` is returned that points to the new entry of `symbols`.
 *
 * This is a convenient helper to register new symbols in `RdsFile` prior to calling `write_rds()` (or to `RdaFile` before `write_rda()`).
 * It returns a `SymbolIndex` that can be used in various fields like `LanguageArgument::name`, `PairListElement::tag`, `EnvironmentVariable::name`, etc.
 */
inline SymbolIndex register_symbol(std::string name, StringEncoding encoding, std::vector<Symbol>& symbols) {
    auto idx = symbols.size();
    symbols.emplace_back(std::move(name), encoding);
    return SymbolIndex(idx);
}

/**
 * @cond
 */
struct Attribute;
/**
 * @endcond
 */

/**
 * @brief Reference to an environment. 
 */
struct EnvironmentIndex final : public RObject {
    /**
     * Default constructor.
     */
    EnvironmentIndex() = default;

    /**
     * @param e Type of the environment, typically one of the special environments (global, empty or base) rather than `SEXPType::ENV`. 
     */
    EnvironmentIndex(SEXPType e) : env_type(e) {}

    /**
     * Create an `EnvironmentIndex` that refers to a non-global environment, i.e., is of type `SEXPType::ENV`.
     *
     * @param i Value of the environment index. 
     */
    EnvironmentIndex(std::size_t i): index(i), env_type(SEXPType::ENV) {}

    /**
     * Type of environment.
     * Most environments will be `ENV` but special environments can be constructed like `SEXPType::GLOBALENV_`.
     */
    SEXPType type() const { return env_type; }

    /**
     * Index of the `RdsFile::external_pointers` or `RdaFile::external_pointers` vector, containing information about this environment.
     * Only used if `type()` returns `ENV`.
     */
    std::size_t index = -1;

    /**
     * Type of the environment, as returned by `type()`.
     */
    SEXPType env_type = SEXPType::GLOBALENV_;
};

/**
 * @brief Variable in an `Environment`.
 */
struct EnvironmentVariable {
    /**
     * Default constructor.
     */
    EnvironmentVariable() = default;

    /**
     * @param name Variable name.
     * @param value Value of the variable.
     */
    EnvironmentVariable(SymbolIndex name, std::unique_ptr<RObject> value) : name(std::move(name)), value(std::move(value)) {}

    /**
     * Variable name.
     */
    SymbolIndex name;
    /**
     * Value of the variable.
     */
    std::unique_ptr<RObject> value;
};

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
    std::size_t parent = -1;

    /**
     * Variables contained within this environment.
     */
    std::vector<EnvironmentVariable> variables;

    /**
     * Additional attributes.
     */
    std::vector<Attribute> attributes;
};

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
    std::vector<Attribute> attributes;
};

/**
 * @brief Reference to an external pointer.
 */
struct ExternalPointerIndex final : public RObject {
    /**
     * Default constructor.
     */
    ExternalPointerIndex() = default;

    /**
     * @param i Value of the external pointer index.
     */
    ExternalPointerIndex(std::size_t i) : index(i) {}


    SEXPType type() const { return SEXPType::EXTPTR; }

    /**
     * Index of the `RdsFile::external_pointers` or `RdaFile::external_pointers` vector, containing information about this external pointer.
     */
    std::size_t index = -1;
};

/**
 * @brief Attribute name and value. 
 */
struct Attribute {
    /**
     * Default constructor.
     */
    Attribute() = default;

    /**
     * @param name Attribute name.
     * @param value Value of the attribute.
     */
    Attribute(SymbolIndex name, std::unique_ptr<RObject> value) : name(std::move(name)), value(std::move(value)) {}

    /**
     * Attribute name.
     */
    SymbolIndex name;

    /**
     * Value of the attribute.
     */
    std::unique_ptr<RObject> value;
};

/**
 * @brief Vector of some atomic type.
 *
 * @tparam ElementType Data type for each element.
 * @tparam stype `SEXPType` flag for the type. 
 */
template<typename ElementType, SEXPType stype>
struct AtomicVector final : public RObject {
    /**
     * @cond
     */
    AtomicVector(std::size_t n = 0) :
        data(sanisizer::cast<I<decltype(data.size())> >(n))
    {}

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
    std::vector<Attribute> attributes;
};

/**
 * @brief Integer vector.
 */
typedef AtomicVector<std::int32_t, SEXPType::INT> IntegerVector;

/**
 * @brief Logical vector.
 */
typedef AtomicVector<std::int32_t, SEXPType::LGL> LogicalVector;

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
 * @brief Single string in a `StringVector`.
 */
struct String {
    /**
     * Default constructor.
     */
    String() = default;

    /**
     * @param value Value of the string.
     * @param encoding Character encoding of the string.
     */
    String(std::string value, StringEncoding encoding) : value(std::move(value)), encoding(encoding) {}

    /**
     * Value of the string.
     * If unset, the string is considered to be missing.
     */
    std::optional<std::string> value;

    /**
     * Character encoding of the string.
     * This is respected even if `value` is unset.
     */
    StringEncoding encoding = StringEncoding::UTF8;
};

/**
 * @brief String vector.
 */
struct StringVector final : public RObject {
    /**
     * @cond
     */
    StringVector(std::size_t n = 0) : data(sanisizer::cast<I<decltype(data.size())> >(n)) {}
    /**
     * @endcond
     */

    SEXPType type() const { return SEXPType::STR; }

    /**
     * Contents of the vector.
     */
    std::vector<String> data;

    /**
     * Additional attributes.
     */
    std::vector<Attribute> attributes;
};

/**
 * @brief Generic vector, i.e., an ordinary R list.
 */
struct GenericVector final : public RObject {
    /**
     * @cond
     */
    GenericVector(std::size_t n = 0) :
        data(sanisizer::cast<I<decltype(data.size())> >(n))
    {}
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
    std::vector<Attribute> attributes;
};

/**
 * @brief Element of a `PairList`.
 */
struct PairListElement {
    /**
     * Default constructor.
     */
    PairListElement() = default;

    /**
     * Construct an untagged element.
     * @param value Value of the element.
     */
    PairListElement(std::unique_ptr<RObject> value) : value(std::move(value)) {}

    /**
     * Construct a tagged element.
     * @param tag Tag of the element.
     * @param value Value of the element.
     */
    PairListElement(SymbolIndex tag, std::unique_ptr<RObject> value) : tag(std::move(tag)), value(std::move(value)) {}

    /**
     * Tag (a.k.a., name) of the pairlist element.
     * If unset, this element is untagged.
     */
    std::optional<SymbolIndex> tag;

    /**
     * Value of the element.
     */
    std::unique_ptr<RObject> value;
};

/**
 * @brief Pairlist, i.e., a linked list.
 */
struct PairList final : public RObject {
    SEXPType type() const { return SEXPType::LIST; }

    /**
     * Contents of the vector.
     */
    std::vector<PairListElement> data; 

    /**
     * Additional attributes.
     */
    std::vector<Attribute> attributes;
};

/**
 * @brief S4 object.
 */
struct S4Object final : public RObject {
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
    std::vector<Attribute> attributes;
};

/**
 * @brief Built-in function.
 */
struct BuiltInFunction final : public RObject {
    SEXPType type() const { return SEXPType::BUILTIN; }

    /**
     * Name of the built-in function.
     */
    std::string name;
};

/**
 * @brief Function argument in a `LanguageObject`.
 */
struct LanguageArgument {
    /**
     * Default constructor.
     */
    LanguageArgument() = default;

    /**
     * Construct a named argument.
     * @param name Name of the argument.
     * @param value Value of the argument.
     */
    LanguageArgument(SymbolIndex name, std::unique_ptr<RObject> value) : name(std::move(name)), value(std::move(value)) {}

    /**
     * Construct an unnamed argument.
     * @param value Value of the argument.
     */
    LanguageArgument(std::unique_ptr<RObject> value) : value(std::move(value)) {}

    /**
     * Name of the argument.
     * If unset, the argument is unnamed.
     */
    std::optional<SymbolIndex> name;

    /**
     * Value of the argument, typically `LanguageObject` or `SymbolIndex` objects.
     * These may also be `AtomicVector` instances of length 1.
     */
    std::unique_ptr<RObject> value;
};

/**
 * @brief Language object, i.e., a function call.
 */
struct LanguageObject final : public RObject {

    SEXPType type() const { return SEXPType::LANG; }

    /**
     * Name of the function.
     */
    SymbolIndex function;

    /**
     * Function arguments, named or unnamed.
     */
    std::vector<LanguageArgument> arguments;

    /**
     * Additional attributes.
     */
    std::vector<Attribute> attributes;
};

/**
 * @brief Expression vector.
 */
struct ExpressionVector final : public RObject {
    /**
     * @cond
     */
    ExpressionVector(std::size_t n = 0) :
        data(sanisizer::cast<I<decltype(data.size())> >(n))
    {}
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
    std::vector<Attribute> attributes;
};

}

#endif
