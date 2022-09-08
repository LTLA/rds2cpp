#include "Rcpp.h"
#include "rds2cpp/parse_rds.hpp"

#include <iostream>

Rcpp::RObject convert(const rds2cpp::RObject* output) {
    if (output->sexp_type == rds2cpp::INT) {
        auto integer = static_cast<const rds2cpp::IntegerVector*>(output);
        const auto& data = integer->data;
        return Rcpp::IntegerVector(data.begin(), data.end());

    } else if (output->sexp_type == rds2cpp::LGL) {
        auto logical = static_cast<const rds2cpp::LogicalVector*>(output);
        const auto& data = logical->data;
        return Rcpp::LogicalVector(data.begin(), data.end());

    } else if (output->sexp_type == rds2cpp::REAL) {
        auto doubled = static_cast<const rds2cpp::DoubleVector*>(output);
        const auto& data = doubled->data;
        return Rcpp::NumericVector(data.begin(), data.end());

    } else if (output->sexp_type == rds2cpp::RAW) {
        auto raw = static_cast<const rds2cpp::RawVector*>(output);
        const auto& data = raw->data;
        return Rcpp::RawVector(data.begin(), data.end());

    } else if (output->sexp_type == rds2cpp::CPLX) {
        auto cplx = static_cast<const rds2cpp::ComplexVector*>(output);
        const auto& data = cplx->data;
        Rcpp::ComplexVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i].r = std::real(data[i]);
            output[i].i = std::imag(data[i]);
        }
        return output;

    } else if (output->sexp_type == rds2cpp::STR) {
        auto chr = static_cast<const rds2cpp::CharacterVector*>(output);
        const auto& data = chr->data;
        Rcpp::StringVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i].missing) {
                output[i] = NA_STRING;
            } else {
                cetype_t enc = CE_UTF8;
                if (data[i].encoding == rds2cpp::String::ASCII) {
                    enc = CE_NATIVE;
                }
                output[i] = Rcpp::String(data[i].value, enc);
            }
        }
        return output;

    } else if (output->sexp_type == rds2cpp::VEC) {
        auto list = static_cast<const rds2cpp::List*>(output);
        const auto& data = list->data;
        Rcpp::List output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i] = convert(data[i].get());
        }
        return output;
    }

    return R_NilValue;
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject parse(std::string file_name) {
    auto output = rds2cpp::parse_rds(file_name);
    return convert(output.object.get());
}
