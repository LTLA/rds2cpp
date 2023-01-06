#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

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
        output.attr("pretend-to-be-a-pairlist") = Rcpp::LogicalVector::create(1);
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
        auto chr = static_cast<const rds2cpp::StringVector*>(input);
        size_t nnodes = chr->data.size();
        Rcpp::StringVector output(nnodes);
        for (size_t i = 0; i < nnodes; ++i) {
            assign_to_string(output, i, chr->data[i], chr->encodings[i], chr->missing[i]);
        }
        add_attributes(chr->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::VEC) {
        auto list = static_cast<const rds2cpp::GenericVector*>(input);
        const auto& data = list->data;
        Rcpp::List output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i] = convert(data[i].get());
        }
        add_attributes(list->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::BUILTIN) {
        auto bif = static_cast<const rds2cpp::BuiltInFunction*>(input);
        Rcpp::List output(1);
        output[0] = Rcpp::CharacterVector::create(bif->name);
        output.attr("pretend-to-be-a-builtin") = Rcpp::LogicalVector::create(1);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::LANG) {
        auto lang = static_cast<const rds2cpp::LanguageObject*>(input);
        Rcpp::List output(2);
        output[0] = Rcpp::CharacterVector::create(lang->function_name);

        size_t nargs = lang->argument_values.size();
        Rcpp::List args(nargs);
        Rcpp::CharacterVector names(nargs);
        for (size_t i = 0; i < nargs; ++i) {
            args[i] = convert(lang->argument_values[i].get());
            names[i] = lang->argument_names[i]; 
        }
        args.attr("arg-names") = names;
        output[1] = args;

        output.attr("pretend-to-be-a-language") = Rcpp::LogicalVector::create(1);
        add_attributes(lang->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::EXPR) {
        auto expr = static_cast<const rds2cpp::ExpressionVector*>(input);
        const auto& data = expr->data;
        Rcpp::List output(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            output[i] = convert(data[i].get());
        }
        output.attr("pretend-to-be-an-expression") = Rcpp::LogicalVector::create(1);
        add_attributes(expr->attributes, output);
        return output;

    // Reference types are generated with a pointer.
    } else if (input->type() == rds2cpp::SEXPType::SYM) {
        auto sdx = static_cast<const rds2cpp::SymbolIndex*>(input);
        return Rcpp::List::create(Rcpp::Named("symbol_id") = Rcpp::IntegerVector::create(sdx->index));

    } else if (input->type() == rds2cpp::SEXPType::ENV) {
        auto edx = static_cast<const rds2cpp::EnvironmentIndex*>(input);
        return Rcpp::List::create(Rcpp::Named("environment_id") = Rcpp::IntegerVector::create(edx->index));

    } else if (input->type() == rds2cpp::SEXPType::EXTPTR) {
        auto epdx = static_cast<const rds2cpp::ExternalPointerIndex*>(input);
        return Rcpp::List::create(Rcpp::Named("external_pointer_id") = Rcpp::IntegerVector::create(epdx->index));

    } else if (input->type() == rds2cpp::SEXPType::GLOBALENV_) {
        return Rcpp::List::create(Rcpp::Named("environment_id") = Rcpp::IntegerVector::create(-1));
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

    // Fetching environments.
    size_t nenvs = output.environments.size();
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
            Rcpp::Named("parent") = Rcpp::IntegerVector::create(env.parent_type == rds2cpp::SEXPType::GLOBALENV_ ? -1 : static_cast<int>(env.parent)),
            Rcpp::Named("locked") = env.locked
        );
        add_attributes(env.attributes, curout);

        all_envs[e] = curout;
    }

    // Fetching external pointers.
    size_t nexts = output.external_pointers.size();
    Rcpp::List all_exts(nexts);
    for (size_t e = 0; e < nexts; ++e) {
        const auto& ext = output.external_pointers[e];

        Rcpp::List output = Rcpp::List::create(
            Rcpp::Named("protection") = convert(ext.protection.get()),
            Rcpp::Named("tag") = convert(ext.tag.get())
        );
        add_attributes(ext.attributes, output);

        all_exts[e] = output;
    }

    // Fetching symbols.
    size_t nsyms = output.symbols.size();
    Rcpp::StringVector all_symb(nsyms);
    for (size_t s = 0; s < nsyms; ++s) {
        const auto& sym = output.symbols[s];
        all_symb[s] = sym.name;
    }

    return Rcpp::List::create(
        Rcpp::Named("value") = convert(output.object.get()),           
        Rcpp::Named("environments") = all_envs,
        Rcpp::Named("symbols") = all_symb,
        Rcpp::Named("external_pointers") = all_exts
    );
}
