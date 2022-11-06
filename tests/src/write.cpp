#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x) {
    std::unique_ptr<rds2cpp::RObject> output;

    if (x.sexp_type() == INTSXP) {
        Rcpp::IntegerVector vec(x);
        auto ptr = new rds2cpp::IntegerVector;
        output.reset(ptr);
        ptr->data.insert(ptr->data.end(), vec.begin(), vec.end());
        return output;
    }

    return output;
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject write(Rcpp::RObject x, std::string file_name) {
    rds2cpp::Parsed output;
    rds2cpp::set_typical_defaults(output);
    output.object = unconvert(x);
    rds2cpp::write_rds(output, file_name);
    return R_NilValue;
}
