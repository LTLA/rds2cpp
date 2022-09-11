#ifndef RDS2CPP_ROBJECT_HPP
#define RDS2CPP_ROBJECT_HPP

#include <vector>
#include <cstdint>
#include <complex>

#include "SEXPType.hpp"
#include "StringEncoding.hpp"

namespace rds2cpp {

struct RObject {
    RObject(SEXPType st) : sexp_type(st) {}
    virtual ~RObject() {}

    // Rule of 5'ing.
    RObject& operator=(const RObject& rhs) = default;
    RObject(const RObject& rhs) = default;
    RObject& operator=(RObject&& rhs) = default;
    RObject(RObject&& rhs) = default;

    SEXPType sexp_type;

    std::vector<std::string> attribute_names;
    std::vector<StringEncoding> attribute_encodings;
    std::vector<std::unique_ptr<RObject> > attribute_values;
};

struct Null : public RObject {
    Null() : RObject(SEXPType::NIL) {}
};

struct Symbol : public RObject {
    Symbol() : RObject(SEXPType::SYM) {}
    std::string name;
    StringEncoding encoding;
};

template<typename ElementType, SEXPType stype>
struct AtomicVector : public RObject {
    AtomicVector(size_t n = 0) : RObject(stype), data(n) {}
    static constexpr SEXPType vector_sexp_type = stype;
    std::vector<ElementType> data;
};

typedef AtomicVector<int32_t, SEXPType::INT> IntegerVector;

typedef AtomicVector<int32_t, SEXPType::LGL> LogicalVector;

typedef AtomicVector<double, SEXPType::REAL> DoubleVector;

typedef AtomicVector<unsigned char, SEXPType::RAW> RawVector;

typedef AtomicVector<std::complex<double>, SEXPType::CPLX> ComplexVector;

struct CharacterVector : public RObject {
    CharacterVector(size_t n = 0) : RObject(SEXPType::STR), data(n), encodings(n), missing(n) {}
    std::vector<std::string> data;
    std::vector<StringEncoding> encodings;
    std::vector<char> missing;
};

struct List : public RObject {
    List(size_t n = 0) : RObject(SEXPType::VEC), data(n) {}
    std::vector<std::unique_ptr<RObject> > data;
};

struct PairList : public RObject {
    PairList() : RObject(SEXPType::LIST) {}

    std::vector<std::unique_ptr<RObject> > data; 
    std::vector<bool> has_tag;
    std::vector<std::string> tag_names;
    std::vector<StringEncoding> tag_encodings;
};

}

#endif
