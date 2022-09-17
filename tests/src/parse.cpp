#include "Rcpp.h"
#include "rds2cpp/parse_rds.hpp"

void assign_to_string(Rcpp::StringVector& output, size_t i, const std::string& value, rds2cpp::StringEncoding encoding) {
    cetype_t enc = CE_UTF8;
    if (encoding == rds2cpp::StringEncoding::ASCII) {
        enc = CE_NATIVE;
    }
    output[i] = Rcpp::String(value, enc);
}

void assign_to_string(Rcpp::StringVector& output, size_t i, const std::string& value, rds2cpp::StringEncoding encoding, bool missing) {
    if (missing) {
        output[i] = NA_STRING;
    } else {
        assign_to_string(output, i, value, encoding);
    }
}

Rcpp::RObject convert(const rds2cpp::RObject*);

template<class Output>
void add_attributes(const rds2cpp::Attributes& input, Output& output) {
    for (size_t a = 0; a < input.names.size(); ++a) {
        output.attr(input.names[a]) = convert(input.values[a].get());
    }
}

Rcpp::RObject convert(const rds2cpp::RObject* input) {
    if (input->type() == rds2cpp::SEXPType::LIST) {
        auto list = static_cast<const rds2cpp::PairList*>(input);

        const auto& data = list->data;
        Rcpp::List data_output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            data_output[i] = convert(data[i].get());
        }

        size_t nnodes = data.size();
        Rcpp::StringVector tag_output(nnodes);
        for (size_t i = 0; i < nnodes; ++i) {
            if (list->has_tag[i]) {
                assign_to_string(tag_output, i, list->tag_names[i], list->tag_encodings[i]);
            } else {
                tag_output[i] = NA_STRING;
            }
        }
 
        auto output = Rcpp::List::create(Rcpp::Named("data") = data_output, Rcpp::Named("tag") = tag_output);
        add_attributes(list->attributes, output);
        return output;
    }

    if (input->type() == rds2cpp::SEXPType::S4) {
        auto s4 = static_cast<const rds2cpp::S4Object*>(input);

        Rcpp::S4 output(s4->class_name);
        for (size_t s = 0; s < s4->attributes.names.size(); ++s) {
            output.slot(s4->attributes.names[s]) = convert(s4->attributes.values[s].get());
        }

        return Rcpp::RObject(output);
    }

    if (input->type() == rds2cpp::SEXPType::INT) {
        auto integer = static_cast<const rds2cpp::IntegerVector*>(input);
        const auto& data = integer->data;
        Rcpp::IntegerVector output(data.begin(), data.end());
        add_attributes(integer->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::LGL) {
        auto logical = static_cast<const rds2cpp::LogicalVector*>(input);
        const auto& data = logical->data;
        Rcpp::LogicalVector output(data.begin(), data.end());
        add_attributes(logical->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::REAL) {
        auto doubled = static_cast<const rds2cpp::DoubleVector*>(input);
        const auto& data = doubled->data;
        Rcpp::NumericVector output(data.begin(), data.end());
        add_attributes(doubled->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::RAW) {
        auto raw = static_cast<const rds2cpp::RawVector*>(input);
        const auto& data = raw->data;
        Rcpp::RawVector output(data.begin(), data.end());
        add_attributes(raw->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::CPLX) {
        auto cplx = static_cast<const rds2cpp::ComplexVector*>(input);
        const auto& data = cplx->data;
        Rcpp::ComplexVector output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i].r = std::real(data[i]);
            output[i].i = std::imag(data[i]);
        }
        add_attributes(cplx->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::STR) {
        auto chr = static_cast<const rds2cpp::CharacterVector*>(input);
        size_t nnodes = chr->data.size();
        Rcpp::StringVector output(nnodes);
        for (size_t i = 0; i < nnodes; ++i) {
            assign_to_string(output, i, chr->data[i], chr->encodings[i], chr->missing[i]);
        }
        add_attributes(chr->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::VEC) {
        auto list = static_cast<const rds2cpp::List*>(input);
        const auto& data = list->data;
        Rcpp::List output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i] = convert(data[i].get());
        }
        add_attributes(list->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::ENV) {
        auto edx = static_cast<const rds2cpp::EnvironmentIndex*>(input);
        return Rcpp::List::create(Rcpp::Named("id") = Rcpp::IntegerVector::create(edx->index));

    } else if (input->type() == rds2cpp::SEXPType::GLOBALENV_) {
        return Rcpp::List::create(Rcpp::Named("id") = Rcpp::IntegerVector::create(-1));
    }

    return R_NilValue;
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject parse(std::string file_name) {
    auto output = rds2cpp::parse_rds(file_name);
    if (output.object == nullptr) {
        return R_NilValue;
    } 

    size_t nenvs = output.environments.size();
    if (nenvs == 0) {
        return convert(output.object.get());
    }

    Rcpp::List all_envs(nenvs);
    for (size_t e = 0; e < nenvs; ++e) {
        const auto& env = output.environments[e];

        Rcpp::List vars(env.variable_names.size());
        Rcpp::CharacterVector varnames(env.variable_names.size());
        for (size_t i = 0; i < env.variable_names.size(); ++i) {
            vars[i] = convert(env.variable_values[i].get());
            assign_to_string(varnames, i, env.variable_names[i], env.variable_encodings[i], false);
        }
        vars.attr("names") = varnames;

        auto curout = Rcpp::List::create(
            Rcpp::Named("variables") = vars,
            Rcpp::Named("parent") = Rcpp::IntegerVector::create(env.parent == static_cast<size_t>(-1) ? -1 : static_cast<int>(env.parent)),
            Rcpp::Named("locked") = env.locked
        );
        add_attributes(env.attributes, curout);

        all_envs[e] = curout;
    }

    return Rcpp::List::create(
        Rcpp::Named("value") = convert(output.object.get()),           
        Rcpp::Named("environments") = all_envs
    );
}
