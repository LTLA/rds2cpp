#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

template<class SourceVector, class HostVector>
std::unique_ptr<rds2cpp::RObject> prepare_simple_vector(const Rcpp::RObject& x) {
    std::unique_ptr<rds2cpp::RObject> output;
    SourceVector vec(x);
    auto ptr = new HostVector;
    output.reset(ptr);
    ptr->data.insert(ptr->data.end(), vec.begin(), vec.end());
    return output;
}

std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x) {
    if (x.sexp_type() == INTSXP) {
        return prepare_simple_vector<Rcpp::IntegerVector, rds2cpp::IntegerVector>(x);

    } else if (x.sexp_type() == REALSXP) {
        return prepare_simple_vector<Rcpp::NumericVector, rds2cpp::DoubleVector>(x);

    } else if (x.sexp_type() == LGLSXP) {
        return prepare_simple_vector<Rcpp::LogicalVector, rds2cpp::LogicalVector>(x);

    } else if (x.sexp_type() == RAWSXP) {
        return prepare_simple_vector<Rcpp::RawVector, rds2cpp::RawVector>(x);

    } else if (x.sexp_type() == CPLXSXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::ComplexVector vec(x);
        auto ptr = new rds2cpp::ComplexVector;
        output.reset(ptr);

        ptr->data.resize(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            ptr->data[i] = std::complex<double>(vec[i].r, vec[i].i);
        }
        return output;

    } else if (x.sexp_type() == STRSXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::StringVector vec(x);
        auto ptr = new rds2cpp::StringVector;
        output.reset(ptr);

        ptr->data.resize(vec.size());
        ptr->encodings.resize(vec.size(), rds2cpp::StringEncoding::UTF8);
        ptr->missing.resize(vec.size(), false);

        for (size_t i = 0; i < vec.size(); ++i) {
            Rcpp::String current(vec[i]);
            if (current == NA_STRING) {
                ptr->missing[i] = true;
                continue;
            } 
            if (current.get_encoding() == CE_NATIVE) {
                ptr->encodings[i] = rds2cpp::StringEncoding::ASCII;
            }
            ptr->data[i] = current.get_cstring();
        }
        return output;
    }

    return std::unique_ptr<rds2cpp::RObject>();
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject write(Rcpp::RObject x, std::string file_name) {
    rds2cpp::RdsFile output;
    output.object = unconvert(x);
    rds2cpp::write_rds(output, file_name);
    return R_NilValue;
}
