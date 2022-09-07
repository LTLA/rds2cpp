#ifndef RDS2CPP_ROBJECT_HPP
#define RDS2CPP_ROBJECT_HPP

#include <vector>
#include <cstdint>

#include "SEXPType.hpp"

namespace rds2cpp {

struct RObject {
    RObject(SEXPType st) : sexp_type(st) {}
    SEXPType sexp_type;
};

template<typename ElementType, SEXPType stype>
struct AtomicVector : public RObject {
    AtomicVector(size_t n = 0) : RObject(stype), data(n) {}
    std::vector<ElementType> data;
};

typedef AtomicVector<int32_t, INT> IntegerVector;

typedef AtomicVector<double, REAL> DoubleVector;

}

#endif
