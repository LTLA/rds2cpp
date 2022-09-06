#ifndef RDS2CPP_ROBJECT_HPP
#define RDS2CPP_ROBJECT_HPP

#include <vector>
#include <cstdint>

namespace rds2cpp {

struct RObject {
    RObject(int st) : sexp_type(st) {}
    int sexp_type;
};

template<typename ElementType, int stype>
struct AtomicVector : public RObject {
    AtomicVector(size_t n = 0) : RObject(stype), data(n) {}
    std::vector<ElementType> data;
};

typedef AtomicVector<int32_t, 13> IntegerVector;

typedef AtomicVector<double, 14> DoubleVector;

}

#endif
