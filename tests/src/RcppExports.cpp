// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// parse
Rcpp::RObject parse(std::string file_name);
RcppExport SEXP _rds2cpp_parse(SEXP file_nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< std::string >::type file_name(file_nameSEXP);
    rcpp_result_gen = Rcpp::wrap(parse(file_name));
    return rcpp_result_gen;
END_RCPP
}
// write
Rcpp::RObject write(Rcpp::RObject x, std::string file_name);
RcppExport SEXP _rds2cpp_write(SEXP xSEXP, SEXP file_nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< Rcpp::RObject >::type x(xSEXP);
    Rcpp::traits::input_parameter< std::string >::type file_name(file_nameSEXP);
    rcpp_result_gen = Rcpp::wrap(write(x, file_name));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_rds2cpp_parse", (DL_FUNC) &_rds2cpp_parse, 1},
    {"_rds2cpp_write", (DL_FUNC) &_rds2cpp_write, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_rds2cpp(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
