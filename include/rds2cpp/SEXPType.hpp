#ifndef RDS2CPP_SEXPTYPE_HPP
#define RDS2CPP_SEXPTYPE_HPP

namespace rds2cpp {

enum class SEXPType : unsigned char {
    // Taken from https://cran.r-project.org/doc/manuals/r-release/R-ints.html#SEXPTYPEs.
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
    S4 = 25,

    // Taken from src/main/serialize.c
    REF = 255,
    NILVALUE_ = 254,
    GLOBALENV_ = 253,
    UNBOUNDVALUE_ = 252,
    MISSINGARG_ = 251,
    BASENAMESPACE_ = 250,
    NAMESPACE = 249,
    PACKAGE = 248,
    PERSIST = 247,

    CLASSREF = 246,
    GENERICREF = 245,
    BCREPDEF = 244,
    BCREPREF = 243,
    EMPTYENV_ = 242,
    BASEENV_ = 241,

    ATTRLANG = 240,
    ATTRLIST = 239,

    ALTREP_ = 	  238
};

}

#endif
