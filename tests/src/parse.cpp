#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

#include <type_traits>
#include <cstddef>
#include <string>
#include <complex>
#include <type_traits>

void assign_to_string(Rcpp::StringVector& output, std::size_t i, const std::string& value, rds2cpp::StringEncoding encoding) {
    cetype_t enc = CE_UTF8;
    if (encoding == rds2cpp::StringEncoding::ASCII) {
        enc = CE_NATIVE;
    }
    output[i] = Rcpp::String(value, enc);
}

Rcpp::RObject convert(const rds2cpp::RObject*);

template<class Output>
void add_attributes(const rds2cpp::Attributes& input, Output& output) {
    const std::size_t num_inputs = input.names.size();
    for (std::size_t a = 0; a < num_inputs; ++a) {
        output.attr(input.names[a]) = convert(input.values[a].get());
    }
}

Rcpp::RObject convert(const rds2cpp::RObject* input) {
    if (input->type() == rds2cpp::SEXPType::LIST) {
        auto list = static_cast<const rds2cpp::PairList*>(input);

        const auto& data = list->data;
        const std::size_t num_data = data.size();
        Rcpp::List data_output(num_data);
        for (std::size_t i = 0; i < num_data; ++i) {
            data_output[i] = convert(data[i].get());
        }

        const std::size_t nnodes = data.size();
        Rcpp::StringVector tag_output(nnodes);
        for (std::size_t i = 0; i < nnodes; ++i) {
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
        const std::size_t num_attributes = s4->attributes.names.size();
        for (std::size_t s = 0; s < num_attributes; ++s) {
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
        const std::size_t num_data = data.size();
        Rcpp::ComplexVector output(num_data);
        for (std::size_t i = 0; i < num_data; ++i) {
            output[i].r = std::real(data[i]);
            output[i].i = std::imag(data[i]);
        }
        add_attributes(cplx->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::STR) {
        auto chr = static_cast<const rds2cpp::StringVector*>(input);
        const std::size_t nnodes = chr->data.size();
        Rcpp::StringVector output(nnodes);
        for (std::size_t i = 0; i < nnodes; ++i) {
            if (chr->missing[i]) {
                output[i] = NA_STRING;
            } else {
                assign_to_string(output, i, chr->data[i], chr->encodings[i]);
            }
        }
        add_attributes(chr->attributes, output);
        return output;

    } else if (input->type() == rds2cpp::SEXPType::VEC) {
        auto list = static_cast<const rds2cpp::GenericVector*>(input);
        const auto& data = list->data;
        const std::size_t num_data = data.size();
        Rcpp::List output(num_data);
        for (std::size_t i = 0; i < num_data; ++i) {
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

        const std::size_t nargs = lang->argument_values.size();
        Rcpp::List args(nargs);
        Rcpp::CharacterVector names(nargs);
        for (std::size_t i = 0; i < nargs; ++i) {
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
        const std::size_t num_data = data.size();
        Rcpp::List output(num_data);
        for (std::size_t i = 0; i < num_data; ++i) {
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

    } else if (input->type() == rds2cpp::SEXPType::BASEENV_) {
        return Rcpp::List::create(Rcpp::Named("environment_id") = Rcpp::IntegerVector::create(-2));

    } else if (input->type() == rds2cpp::SEXPType::EMPTYENV_) {
        return Rcpp::List::create(Rcpp::Named("environment_id") = Rcpp::IntegerVector::create(-3));
    }

    return R_NilValue;
}

template<typename RdxFile_>
Rcpp::RObject parse_rdx(const RdxFile_& rdx_file) {
    if constexpr(std::is_same<RdxFile_, rds2cpp::RdsFile>::value) {
        if (rdx_file.object == nullptr) {
            return R_NilValue;
        } 
    }

    // Fetching environments.
    std::size_t nenvs = rdx_file.environments.size();
    Rcpp::List all_envs(nenvs);
    for (std::size_t e = 0; e < nenvs; ++e) {
        const auto& env = rdx_file.environments[e];

        Rcpp::List vars(env.variable_names.size());
        Rcpp::CharacterVector varnames(env.variable_names.size());
        const std::size_t var_names = env.variable_names.size();
        for (std::size_t i = 0; i < var_names; ++i) {
            vars[i] = convert(env.variable_values[i].get());
            assign_to_string(varnames, i, env.variable_names[i], env.variable_encodings[i]);
        }
        vars.attr("names") = varnames;

        int parent_code = 0;
        if (env.parent_type == rds2cpp::SEXPType::ENV) {
            parent_code = static_cast<int>(env.parent);
        } else if (env.parent_type == rds2cpp::SEXPType::GLOBALENV_) {
            parent_code = -1;
        } else if (env.parent_type == rds2cpp::SEXPType::BASEENV_) {
            parent_code = -2;
        } else if (env.parent_type == rds2cpp::SEXPType::EMPTYENV_) {
            parent_code = -3;
        } else {
            throw std::runtime_error("oops, don't know how to handle a parent type of " + std::to_string(static_cast<int>(env.parent_type)));
        }

        auto curout = Rcpp::List::create(
            Rcpp::Named("variables") = vars,
            Rcpp::Named("parent") = parent_code,
            Rcpp::Named("locked") = env.locked
        );
        add_attributes(env.attributes, curout);

        all_envs[e] = curout;
    }

    // Fetching external pointers.
    const std::size_t nexts = rdx_file.external_pointers.size();
    Rcpp::List all_exts(nexts);
    for (std::size_t e = 0; e < nexts; ++e) {
        const auto& ext = rdx_file.external_pointers[e];

        Rcpp::List rdx_file = Rcpp::List::create(
            Rcpp::Named("protection") = convert(ext.protection.get()),
            Rcpp::Named("tag") = convert(ext.tag.get())
        );
        add_attributes(ext.attributes, rdx_file);

        all_exts[e] = rdx_file;
    }

    // Fetching symbols.
    const std::size_t nsyms = rdx_file.symbols.size();
    Rcpp::StringVector all_symb(nsyms);
    for (std::size_t s = 0; s < nsyms; ++s) {
        const auto& sym = rdx_file.symbols[s];
        all_symb[s] = sym.name;
    }

    auto output = Rcpp::List::create(
        Rcpp::Named("environments") = all_envs,
        Rcpp::Named("symbols") = all_symb,
        Rcpp::Named("external_pointers") = all_exts
    );

    output["format_version"] = rdx_file.format_version;

    output["writer_version"] = Rcpp::IntegerVector::create(
        rdx_file.writer_version.major,
        rdx_file.writer_version.minor,
        rdx_file.writer_version.patch
    );

    output["reader_version"] = Rcpp::IntegerVector::create(
        rdx_file.reader_version.major,
        rdx_file.reader_version.minor,
        rdx_file.reader_version.patch
    );

    output["string_encoding"] = rds2cpp::string_encoding_to_name(rdx_file.encoding);

    if constexpr(std::is_same<RdxFile_, rds2cpp::RdsFile>::value) {
        output["value"] = convert(rdx_file.object.get());
    } else {
        output["value"] = convert(&(rdx_file.contents));
    }

    return output;
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject parse_rds(std::string file_name, bool parallel) {
    rds2cpp::ParseRdsOptions opts;
    opts.parallel = parallel;
    auto parsed = rds2cpp::parse_rds(file_name, opts);
    return parse_rdx(parsed);
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject parse_rda(std::string file_name, bool parallel) {
    rds2cpp::ParseRdaOptions opts;
    opts.parallel = parallel;
    auto parsed = rds2cpp::parse_rda(file_name, opts);
    return parse_rdx(parsed);
}

//' @export
//[[Rcpp::export(rng=false)]]
double parse_length(Rcpp::RawVector raw) {
    byteme::RawBufferReader reader(raw.begin(), raw.size());
    byteme::SerialBufferedReader<unsigned char, byteme::Reader*> src(&reader, 100);
    return rds2cpp::get_length(src);
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::List parse_single_string(Rcpp::RawVector raw) {
    byteme::RawBufferReader reader(raw.begin(), raw.size());
    byteme::SerialBufferedReader<unsigned char, byteme::Reader*> src(&reader, 100);
    auto payload = rds2cpp::parse_single_string(src);
    const std::string encoding = rds2cpp::string_encoding_to_name(payload.encoding);
    return Rcpp::List::create(payload.value, encoding, payload.missing);
}
