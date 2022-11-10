#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x);

template<class RdsObject>
void add_attributes(const Rcpp::RObject& x, RdsObject* y) {
    const auto& attr_names = x.attributeNames();
    auto& attr_dest = y->attributes;
    for (const auto& attr : attr_names) {
        attr_dest.names.push_back(attr);        
        attr_dest.encodings.push_back(rds2cpp::StringEncoding::ASCII); // just assume, I guess.
        attr_dest.values.push_back(unconvert(x.attr(attr)));
    }
}

template<class SourceVector, class HostVector>
std::unique_ptr<rds2cpp::RObject> prepare_simple_vector(const Rcpp::RObject& x) {
    std::unique_ptr<rds2cpp::RObject> output;
    SourceVector vec(x);
    auto ptr = new HostVector;
    output.reset(ptr);
    ptr->data.insert(ptr->data.end(), vec.begin(), vec.end());
    add_attributes(x, ptr);
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

        add_attributes(x, ptr);
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

        add_attributes(x, ptr);
        return output;

    } else if (x.sexp_type() == VECSXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::List vec(x);

        if (vec.hasAttribute("pretend-to-be-a-pairlist")) {
            auto ptr = new rds2cpp::PairList;
            output.reset(ptr);

            bool has_names = vec.hasAttribute("names");
            Rcpp::CharacterVector names;
            if (has_names) {
                names = vec.attr("names");
            }

            for (size_t i = 0; i < vec.size(); ++i) {
                ptr->data.push_back(unconvert(vec[i]));
                std::string curname;
                if (has_names) {
                    curname = Rcpp::String(names[i]).get_cstring();
                }
                ptr->has_tag.push_back(curname != "");
                ptr->tag_names.push_back(curname);
                ptr->tag_encodings.push_back(rds2cpp::StringEncoding::UTF8);
            }
            add_attributes(x, ptr);

        } else {
            auto ptr = new rds2cpp::GenericVector;
            output.reset(ptr);
            for (size_t i = 0; i < vec.size(); ++i) {
                ptr->data.push_back(unconvert(vec[i]));
            }
            add_attributes(x, ptr);
        }

        return output;

    } else if (x.sexp_type() == S4SXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::S4 obj(x);
        auto ptr = new rds2cpp::S4Object;
        output.reset(ptr);

        const auto& names = obj.attributeNames();
        for (const auto& n : names) {
            if (n != "class") {
                ptr->attributes.names.push_back(n);
                ptr->attributes.encodings.push_back(rds2cpp::StringEncoding::ASCII); // again, just assuming.
                ptr->attributes.values.push_back(unconvert(obj.slot(n)));
            }
        }

        Rcpp::CharacterVector cls = obj.attr("class");
        ptr->class_name = Rcpp::String(cls[0]).get_cstring();
        ptr->class_encoding = rds2cpp::StringEncoding::ASCII; // again, just assuming.

        Rcpp::CharacterVector pkg = cls.attr("package");
        ptr->package_name = Rcpp::String(pkg[0]).get_cstring();
        ptr->package_encoding = rds2cpp::StringEncoding::ASCII; // again, just assuming.

        return output;

    } else if (x.sexp_type() == NILSXP) {
        return std::unique_ptr<rds2cpp::RObject>(new rds2cpp::Null);
    }

    throw std::runtime_error("unsupported SEXP type '" + std::to_string(static_cast<int>(x.sexp_type())) + "' for writing");
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
