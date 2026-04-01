#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

#include <memory>
#include <unordered_set>
#include <stdexcept>
#include <string>
#include <cstddef>

template<class RdxFile_>
std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x, RdxFile_& globals);

template<class RdsObject_, class RdxFile_>
void add_attributes(const Rcpp::RObject& x, RdsObject_* y, RdxFile_& globals) {
    const auto& attr_names = x.attributeNames();
    auto& attr_dest = y->attributes;
    for (const auto& attr : attr_names) {
        attr_dest.emplace_back(
            rds2cpp::register_symbol(attr, rds2cpp::StringEncoding::ASCII, globals.symbols), // just assume the encoding, I guess.
            unconvert(x.attr(attr), globals)
        );
    }
}

template<class RdsObject_, class RdxFile_>
void add_attributes_except(const Rcpp::RObject& x, RdsObject_* y, RdxFile_& globals, const std::unordered_set<std::string>& excluded) {
    const auto& attr_names = x.attributeNames();
    auto& attr_dest = y->attributes;
    for (const auto& attr : attr_names) {
        if (excluded.find(attr) == excluded.end()) {
            attr_dest.emplace_back(
                rds2cpp::register_symbol(attr, rds2cpp::StringEncoding::ASCII, globals.symbols), // just assume the encoding, I guess.
                unconvert(x.attr(attr), globals)
            );
        }
    }
}

template<class SourceVector_, class HostVector_, class RdxFile_>
std::unique_ptr<rds2cpp::RObject> prepare_simple_vector(const Rcpp::RObject& x, RdxFile_& globals) {
    std::unique_ptr<rds2cpp::RObject> output;
    SourceVector_ vec(x);
    auto ptr = new HostVector_;
    output.reset(ptr);
    ptr->data.insert(ptr->data.end(), vec.begin(), vec.end());
    add_attributes(x, ptr, globals);
    return output;
}

template<class RdxFile_>
std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x, RdxFile_& globals) {
    if (x.sexp_type() == INTSXP) {
        return prepare_simple_vector<Rcpp::IntegerVector, rds2cpp::IntegerVector>(x, globals);

    } else if (x.sexp_type() == REALSXP) {
        return prepare_simple_vector<Rcpp::NumericVector, rds2cpp::DoubleVector>(x, globals);

    } else if (x.sexp_type() == LGLSXP) {
        return prepare_simple_vector<Rcpp::LogicalVector, rds2cpp::LogicalVector>(x, globals);

    } else if (x.sexp_type() == RAWSXP) {
        return prepare_simple_vector<Rcpp::RawVector, rds2cpp::RawVector>(x, globals);

    } else if (x.sexp_type() == CPLXSXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::ComplexVector vec(x);
        auto ptr = new rds2cpp::ComplexVector;
        output.reset(ptr);

        const std::size_t n = vec.size();
        ptr->data.resize(n);
        for (std::size_t i = 0; i < n; ++i) {
            ptr->data[i] = std::complex<double>(vec[i].r, vec[i].i);
        }

        add_attributes(x, ptr, globals);
        return output;

    } else if (x.sexp_type() == STRSXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::StringVector vec(x);

        if (vec.size() == 1 && vec.hasAttribute("pretend-to-be-a-symbol")) {
            output.reset(new rds2cpp::SymbolIndex(globals.symbols.size()));
            Rcpp::String current(vec[0]);
            globals.symbols.emplace_back(current.get_cstring(), rds2cpp::StringEncoding::ASCII);

        } else {
            auto ptr = new rds2cpp::StringVector;
            output.reset(ptr);

            const std::size_t n = vec.size();
            ptr->data.resize(n);

            for (std::size_t i = 0; i < n; ++i) {
                Rcpp::String current(vec[i]);
                if (current == NA_STRING) {
                    continue;
                } 
                const auto enc = current.get_encoding();
                if (enc == CE_NATIVE) {
                    ptr->data[i].encoding = rds2cpp::StringEncoding::ASCII;
                } else if (enc == CE_LATIN1) {
                    ptr->data[i].encoding = rds2cpp::StringEncoding::LATIN1;
                }
                ptr->data[i].value = current.get_cstring();
            }

            add_attributes(x, ptr, globals);
        }

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

            const std::size_t n = vec.size();
            for (std::size_t i = 0; i < n; ++i) {
                auto unc = unconvert(vec[i], globals);
                std::string curname;
                if (has_names) {
                    curname = Rcpp::String(names[i]).get_cstring();
                }
                if (curname == "") {
                    ptr->data.emplace_back(std::move(unc));
                } else {
                    ptr->data.emplace_back(
                        rds2cpp::register_symbol(std::move(curname), rds2cpp::StringEncoding::ASCII, globals.symbols),
                        std::move(unc)
                    );
                }
            }
            add_attributes_except(x, ptr, globals, { "pretend-to-be-a-pairlist", "names" });

        } else if (vec.hasAttribute("pretend-to-be-an-environment")) {
            Rcpp::IntegerVector env_index = vec.attr("environment-index");
            auto index = env_index[0];

            auto ptr = new rds2cpp::EnvironmentIndex;
            output.reset(ptr);
            ptr->index = index;

            if (index >= 0) {
                ptr->env_type = rds2cpp::SEXPType::ENV;
                if (static_cast<std::size_t>(index) == globals.environments.size()) {
                    rds2cpp::Environment latest;

                    Rcpp::IntegerVector env_parent = vec.attr("environment-parent");
                    auto parent = env_parent[0];
                    if (parent >= 0) {
                        latest.parent = parent;
                        latest.parent_type = rds2cpp::SEXPType::ENV;
                    } else {
                        latest.parent = -1;
                        latest.parent_type = rds2cpp::SEXPType::GLOBALENV_;
                    }

                    Rcpp::LogicalVector is_locked = vec.attr("environment-locked");
                    latest.locked = is_locked[0];

                    Rcpp::CharacterVector names = vec.attr("names");
                    const std::size_t n = vec.size();
                    for (std::size_t i = 0; i < n; ++i) {
                        latest.variables.emplace_back(
                            rds2cpp::register_symbol(Rcpp::String(names[i]).get_cstring(), rds2cpp::StringEncoding::UTF8, globals.symbols),
                            unconvert(vec[i], globals)
                        );
                    }

                    add_attributes_except(x, &latest, globals, { "pretend-to-be-an-environment", "environment-index", "environment-parent", "environment-locked", "names" });
                    globals.environments.push_back(std::move(latest));
                } else if (static_cast<std::size_t>(index) > globals.environments.size()) {
                    throw std::runtime_error("environment index out of range");
                }

            } else if (index == -1) {
                ptr->env_type = rds2cpp::SEXPType::GLOBALENV_;

            } else if (index == -2) {
                ptr->env_type = rds2cpp::SEXPType::BASEENV_;

            } else if (index == -3) {
                ptr->env_type = rds2cpp::SEXPType::EMPTYENV_;

            } else {
                throw std::runtime_error("unknown special environment index " + std::to_string(index));
            }

        } else if (vec.hasAttribute("pretend-to-be-a-builtin")) {
            auto ptr = new rds2cpp::BuiltInFunction;
            output.reset(ptr);
            Rcpp::CharacterVector name(vec[0]);
            ptr->name = Rcpp::String(name[0]).get_cstring();

        } else if (vec.hasAttribute("pretend-to-be-a-language")) {
            auto ptr = new rds2cpp::LanguageObject;
            output.reset(ptr);
            Rcpp::CharacterVector name(vec[0]);
            ptr->function = rds2cpp::register_symbol(Rcpp::String(name[0]).get_cstring(), rds2cpp::StringEncoding::UTF8, globals.symbols);

            Rcpp::List arguments(vec[1]);
            Rcpp::CharacterVector argnames;
            if (arguments.hasAttribute("names")) {
                argnames = arguments.names();
            }

            const std::size_t num_args = arguments.size();
            for (std::size_t a = 0; a < num_args; ++a) {
                auto unc = unconvert(arguments[a], globals);
                if (argnames.size() && argnames[a] != NA_STRING) {
                    ptr->arguments.emplace_back(
                        rds2cpp::register_symbol(Rcpp::String(argnames[a]).get_cstring(), rds2cpp::StringEncoding::UTF8, globals.symbols),
                        std::move(unc)
                    );
                } else {
                    ptr->arguments.emplace_back(std::move(unc));
                }
            }
            add_attributes_except(x, ptr, globals, { "pretend-to-be-a-language" });

        } else if (vec.hasAttribute("pretend-to-be-an-expression")) {
            auto ptr = new rds2cpp::ExpressionVector;
            output.reset(ptr);
            for (const auto& v : vec) {
                ptr->data.push_back(unconvert(v, globals));
            }
            add_attributes_except(x, ptr, globals, { "pretend-to-be-an-expression" });

        } else if (vec.hasAttribute("pretend-to-be-an-external-pointer")) {
            Rcpp::IntegerVector extptr_index = vec.attr("external-pointer-index");
            auto index = extptr_index[0];

            auto ptr = new rds2cpp::ExternalPointerIndex;
            output.reset(ptr);
            ptr->index = index;

            if (static_cast<std::size_t>(index) == globals.external_pointers.size()) {
                rds2cpp::ExternalPointer latest;
                latest.protection = unconvert(vec[0], globals);
                latest.tag = unconvert(vec[1], globals);
                add_attributes_except(x, &latest, globals, { "pretend-to-be-an-external-pointer", "external-pointer-index" });
                globals.external_pointers.push_back(std::move(latest));
            } else if (static_cast<std::size_t>(index) > globals.external_pointers.size()) {
                throw std::runtime_error("environment index out of range");
            }

        } else {
            auto ptr = new rds2cpp::GenericVector;
            output.reset(ptr);
            for (const auto& v : vec) {
                ptr->data.push_back(unconvert(v, globals));
            }
            add_attributes(x, ptr, globals);
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
                ptr->attributes.emplace_back(
                    rds2cpp::register_symbol(n, rds2cpp::StringEncoding::ASCII, globals.symbols), // again, just assuming the encoding.
                    unconvert(obj.slot(n), globals)
                );
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
Rcpp::RObject write_rds(Rcpp::RObject x, std::string file_name, bool parallel) {
    rds2cpp::RdsFile output;
    output.object = unconvert(x, output);
    rds2cpp::WriteRdsOptions opt;
    opt.parallel = parallel;
    rds2cpp::write_rds(output, file_name, opt);
    return R_NilValue;
}

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::RObject write_rda(Rcpp::List x, std::string file_name, bool parallel) {
    rds2cpp::RdaFile output;

    output.objects.reserve(x.size());
    const std::size_t n = x.size();
    Rcpp::CharacterVector names = x.attr("names");
    for (std::size_t i = 0; i < n; ++i) {
        output.objects.emplace_back(
            rds2cpp::register_symbol(Rcpp::as<std::string>(names[i]), rds2cpp::StringEncoding::UTF8, output.symbols),
            unconvert(x[i], output)
        );
    }

    rds2cpp::WriteRdaOptions opt;
    opt.parallel = parallel;
    rds2cpp::write_rda(output, file_name, opt);
    return R_NilValue;
}
