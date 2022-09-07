#include "Rcpp.h"
#include "rds2cpp/parse_rds.hpp"

#include <iostream>

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject parse(std::string file_name) {
    auto output = rds2cpp::parse_rds(file_name);

    if (output.object->sexp_type == rds2cpp::INT) {
        auto integer = static_cast<const rds2cpp::IntegerVector*>(output.object.get());
        const auto& data = integer->data;
        return Rcpp::IntegerVector(data.begin(), data.end());

    } else if (output.object->sexp_type == rds2cpp::LGL) {
        auto logical = static_cast<const rds2cpp::LogicalVector*>(output.object.get());
        const auto& data = logical->data;
        return Rcpp::LogicalVector(data.begin(), data.end());

    } else if (output.object->sexp_type == rds2cpp::REAL) {
        auto doubled = static_cast<const rds2cpp::DoubleVector*>(output.object.get());
        const auto& data = doubled->data;
        return Rcpp::NumericVector(data.begin(), data.end());

    } else if (output.object->sexp_type == rds2cpp::RAW) {
        auto raw = static_cast<const rds2cpp::RawVector*>(output.object.get());
        const auto& data = raw->data;
        return Rcpp::RawVector(data.begin(), data.end());

    } else if (output.object->sexp_type == rds2cpp::CPLX) {
        auto cplx = static_cast<const rds2cpp::ComplexVector*>(output.object.get());
        const auto& data = cplx->data;
        Rcpp::ComplexVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i].r = std::real(data[i]);
            output[i].i = std::imag(data[i]);
        }
        return output;

    } else if (output.object->sexp_type == rds2cpp::STR) {
        auto chr = static_cast<const rds2cpp::CharacterVector*>(output.object.get());
        const auto& data = chr->data;
        Rcpp::StringVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i].first) {
                output[i] = NA_STRING;
            } else {
                output[i] = data[i].second;
            }
        }
        return output;
    }

    return R_NilValue;
}
