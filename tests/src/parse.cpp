#include "Rcpp.h"
#include "rds2cpp/parse_rds.hpp"

void assign_to_string(Rcpp::StringVector& output, size_t i, const rds2cpp::String& thing) {
    if (thing.missing) {
        output[i] = NA_STRING;
    } else {
        cetype_t enc = CE_UTF8;
        if (thing.encoding == rds2cpp::String::ASCII) {
            enc = CE_NATIVE;
        }
        output[i] = Rcpp::String(thing.value, enc);
    }
}

Rcpp::RObject convert(const rds2cpp::RObject*);

template<class Output>
void add_attributes(const rds2cpp::RObject& input, Output& output) {
    for (size_t a = 0; a < input.attribute_names.size(); ++a) {
        output.attr(input.attribute_names[a]) = convert(input.attribute_values[a].get());
    }
}

Rcpp::RObject convert(const rds2cpp::RObject* input) {
    if (input->sexp_type == rds2cpp::LIST) {
        auto list = static_cast<const rds2cpp::PairList*>(input);

        const auto& data = list->data;
        Rcpp::List data_output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            data_output[i] = convert(data[i].get());
        }

        const auto& tags = list->tags;
        Rcpp::StringVector tag_output(tags.size());
        for (size_t i = 0; i < data.size(); ++i) {
            if (tags[i].first) {
                assign_to_string(tag_output, i, tags[i].second);
            } else {
                tag_output[i] = NA_STRING;
            }
        }
 
        auto output = Rcpp::List::create(Rcpp::Named("data") = data_output, Rcpp::Named("tag") = tag_output);
        add_attributes(*input, output);
        return output;
    }

    if (input->sexp_type == rds2cpp::INT) {
        auto integer = static_cast<const rds2cpp::IntegerVector*>(input);
        const auto& data = integer->data;
        Rcpp::IntegerVector output(data.begin(), data.end());
        add_attributes(*input, output);
        return output;

    } else if (input->sexp_type == rds2cpp::LGL) {
        auto logical = static_cast<const rds2cpp::LogicalVector*>(input);
        const auto& data = logical->data;
        Rcpp::LogicalVector output(data.begin(), data.end());
        add_attributes(*input, output);
        return output;

    } else if (input->sexp_type == rds2cpp::REAL) {
        auto doubled = static_cast<const rds2cpp::DoubleVector*>(input);
        const auto& data = doubled->data;
        Rcpp::NumericVector output(data.begin(), data.end());
        add_attributes(*input, output);
        return output;

    } else if (input->sexp_type == rds2cpp::RAW) {
        auto raw = static_cast<const rds2cpp::RawVector*>(input);
        const auto& data = raw->data;
        Rcpp::RawVector output(data.begin(), data.end());
        add_attributes(*input, output);
        return output;

    } else if (input->sexp_type == rds2cpp::CPLX) {
        auto cplx = static_cast<const rds2cpp::ComplexVector*>(input);
        const auto& data = cplx->data;
        Rcpp::ComplexVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i].r = std::real(data[i]);
            output[i].i = std::imag(data[i]);
        }
        add_attributes(*input, output);
        return output;

    } else if (input->sexp_type == rds2cpp::STR) {
        auto chr = static_cast<const rds2cpp::CharacterVector*>(input);
        const auto& data = chr->data;
        Rcpp::StringVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            assign_to_string(output, i, data[i]);
        }
        add_attributes(*input, output);
        return output;

    } else if (input->sexp_type == rds2cpp::VEC) {
        auto list = static_cast<const rds2cpp::List*>(input);
        const auto& data = list->data;
        Rcpp::List output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i] = convert(data[i].get());
        }
        add_attributes(*input, output);
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
