#ifndef RDS2CPP_SEXPTYPE_HPP
#define RDS2CPP_SEXPTYPE_HPP

namespace rds2cpp {

enum class SEXPType {
    NIL = 0,
    SYM = 1,
    LIST = 2,
    CLO = 3,
    ENV = 4,
    PROM = 5,
    LANG = 6,
    SPECIAL = 7,
    BUILTIN = 8,
    CHAR = 9,
    LGL = 10,
    INT = 13,
    REAL = 14,
    CPLX = 15,
    STR = 16,
    DOT = 17,
    ANY = 18,
    VEC = 19,
    EXPR = 20,
    BCODE = 21,
    EXTPTR = 22,
    WEAKREF = 23,
    RAW = 24,
    S4 = 25
};

}

#endif
