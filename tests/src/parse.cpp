#include "Rcpp.h"
#include "rds2cpp/parse_rds.hpp"

#include <iostream>

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject parse(std::string file_name) {
    auto output = rds2cpp::parse_rds(file_name);

    if (output.object->sexp_type == 13) {
        auto integer = static_cast<const rds2cpp::IntegerVector*>(output.object.get());
        const auto& data = integer->data;
        return Rcpp::IntegerVector(data.begin(), data.end());
    }

    return R_NilValue;
}
