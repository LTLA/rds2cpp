#ifndef RDS2CPP_ROBJECT_HPP
#define RDS2CPP_ROBJECT_HPP

#include <vector>
#include <cstdint>
#include <complex>

#include "SEXPType.hpp"

namespace rds2cpp {

struct String {
    std::string value;

    enum Encoding { NONE, LATIN1, UTF8, ASCII };

    Encoding encoding = NONE;

    bool missing;
};

struct RObject {
    RObject(SEXPType st) : sexp_type(st) {}
    SEXPType sexp_type;

    std::vector<std::string> attribute_names;
    std::vector<String::Encoding> attribute_encodings;
    std::vector<std::shared_ptr<RObject> > attribute_values;
};

template<typename ElementType, SEXPType stype>
struct AtomicVector : public RObject {
    AtomicVector(size_t n = 0) : RObject(stype), data(n) {}
    std::vector<ElementType> data;
};

typedef AtomicVector<int32_t, INT> IntegerVector;

typedef AtomicVector<int32_t, LGL> LogicalVector;

typedef AtomicVector<double, REAL> DoubleVector;

typedef AtomicVector<unsigned char, RAW> RawVector;

typedef AtomicVector<std::complex<double>, CPLX> ComplexVector;

typedef AtomicVector<String, STR> CharacterVector;

struct List : public RObject {
    List(size_t n = 0) : RObject(VEC), data(n) {}
    std::vector<std::shared_ptr<RObject> > data;
};

struct PairList : public RObject {
    PairList() : RObject(LIST) {}
    std::vector<std::shared_ptr<RObject> > data;  
    std::vector<std::pair<bool, String> > tags;
};

}

#endif
