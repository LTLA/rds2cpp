#include "Rcpp.h"
#include "rds2cpp/rds2cpp.hpp"

std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x, rds2cpp::RdsFile& globals);

template<class RdsObject>
void add_attributes(const Rcpp::RObject& x, RdsObject* y, rds2cpp::RdsFile& globals) {
    const auto& attr_names = x.attributeNames();
    auto& attr_dest = y->attributes;
    for (const auto& attr : attr_names) {
        attr_dest.names.push_back(attr);        
        attr_dest.encodings.push_back(rds2cpp::StringEncoding::ASCII); // just assume, I guess.
        attr_dest.values.push_back(unconvert(x.attr(attr), globals));
    }
}

template<class RdsObject>
void add_attributes_except(const Rcpp::RObject& x, RdsObject* y, rds2cpp::RdsFile& globals, const std::unordered_set<std::string>& excluded) {
    const auto& attr_names = x.attributeNames();
    auto& attr_dest = y->attributes;
    for (const auto& attr : attr_names) {
        if (excluded.find(attr) == excluded.end()) {
            attr_dest.names.push_back(attr);
            attr_dest.encodings.push_back(rds2cpp::StringEncoding::ASCII); 
            attr_dest.values.push_back(unconvert(x.attr(attr), globals));
        }
    }
}

template<class SourceVector, class HostVector>
std::unique_ptr<rds2cpp::RObject> prepare_simple_vector(const Rcpp::RObject& x, rds2cpp::RdsFile& globals) {
    std::unique_ptr<rds2cpp::RObject> output;
    SourceVector vec(x);
    auto ptr = new HostVector;
    output.reset(ptr);
    ptr->data.insert(ptr->data.end(), vec.begin(), vec.end());
    add_attributes(x, ptr, globals);
    return output;
}

std::unique_ptr<rds2cpp::RObject> unconvert(const Rcpp::RObject& x, rds2cpp::RdsFile& globals) {
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

        ptr->data.resize(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            ptr->data[i] = std::complex<double>(vec[i].r, vec[i].i);
        }

        add_attributes(x, ptr, globals);
        return output;

    } else if (x.sexp_type() == STRSXP) {
        std::unique_ptr<rds2cpp::RObject> output;
        Rcpp::StringVector vec(x);

        if (vec.size() == 1 && vec.hasAttribute("pretend-to-be-a-symbol")) {
            auto ptr = new rds2cpp::SymbolIndex;
            output.reset(ptr);
            ptr->index = globals.symbols.size();
            Rcpp::String current(vec[0]);

            rds2cpp::Symbol another;
            another.name = current.get_cstring();
            another.encoding = rds2cpp::StringEncoding::ASCII;
            globals.symbols.push_back(std::move(another));

        } else {
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

            for (size_t i = 0; i < vec.size(); ++i) {
                ptr->data.push_back(unconvert(vec[i], globals));
                std::string curname;
                if (has_names) {
                    curname = Rcpp::String(names[i]).get_cstring();
                }
                ptr->has_tag.push_back(curname != "");
                ptr->tag_names.push_back(curname);
                ptr->tag_encodings.push_back(rds2cpp::StringEncoding::UTF8);
            }
            add_attributes(x, ptr, globals);

        } else if (vec.hasAttribute("pretend-to-be-an-environment")) {
            Rcpp::IntegerVector env_index = vec.attr("environment-index");
            auto index = env_index[0];

            auto ptr = new rds2cpp::EnvironmentIndex;
            output.reset(ptr);
            ptr->index = index;

            if (index >= 0) {
                ptr->env_type = rds2cpp::SEXPType::ENV;
                if (static_cast<size_t>(index) == globals.environments.size()) {
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
                    for (size_t i = 0; i < vec.size(); ++i) {
                        latest.variable_names.emplace_back(Rcpp::String(names[i]).get_cstring());
                        latest.variable_encodings.push_back(rds2cpp::StringEncoding::UTF8);
                        latest.variable_values.push_back(unconvert(vec[i], globals));
                    }

                    add_attributes_except(x, &latest, globals, { "pretend-to-be-an-environment", "environment-index", "environment-parent", "environment-locked", "names" });
                    globals.environments.push_back(std::move(latest));
                } else if (static_cast<size_t>(index) > globals.environments.size()) {
                    throw std::runtime_error("environment index out of range");
                }
            } else {
                ptr->env_type = rds2cpp::SEXPType::GLOBALENV_;
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
            ptr->function_name = Rcpp::String(name[0]).get_cstring();

            Rcpp::List arguments(vec[1]);
            Rcpp::CharacterVector argnames;
            if (arguments.hasAttribute("names")) {
                argnames = arguments.names();
            }

            for (size_t a = 0; a < arguments.size(); ++a) {
                if (argnames.size()) {
                    std::string candidate = Rcpp::String(argnames[a]).get_cstring();
                    if (!candidate.empty()) {
                        ptr->add_argument(candidate, unconvert(arguments[a], globals));
                        continue;
                    }
                }
                ptr->add_argument(unconvert(arguments[a], globals));
            }
            add_attributes_except(x, ptr, globals, { "pretend-to-be-a-language" });

        } else if (vec.hasAttribute("pretend-to-be-an-expression")) {
            auto ptr = new rds2cpp::ExpressionVector;
            output.reset(ptr);
            for (size_t i = 0; i < vec.size(); ++i) {
                ptr->data.push_back(unconvert(vec[i], globals));
            }
            add_attributes_except(x, ptr, globals, { "pretend-to-be-an-expression" });

        } else if (vec.hasAttribute("pretend-to-be-an-external-pointer")) {
            Rcpp::IntegerVector extptr_index = vec.attr("external-pointer-index");
            auto index = extptr_index[0];

            auto ptr = new rds2cpp::ExternalPointerIndex;
            output.reset(ptr);
            ptr->index = index;

            if (static_cast<size_t>(index) == globals.external_pointers.size()) {
                rds2cpp::ExternalPointer latest;
                latest.protection = unconvert(vec[0], globals);
                latest.tag= unconvert(vec[1], globals);
                add_attributes_except(x, &latest, globals, { "pretend-to-be-an-external-pointer", "external-pointer-index" });
                globals.external_pointers.push_back(std::move(latest));
            } else if (static_cast<size_t>(index) > globals.external_pointers.size()) {
                throw std::runtime_error("environment index out of range");
            }

        } else {
            auto ptr = new rds2cpp::GenericVector;
            output.reset(ptr);
            for (size_t i = 0; i < vec.size(); ++i) {
                ptr->data.push_back(unconvert(vec[i], globals));
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
                ptr->attributes.names.push_back(n);
                ptr->attributes.encodings.push_back(rds2cpp::StringEncoding::ASCII); // again, just assuming.
                ptr->attributes.values.push_back(unconvert(obj.slot(n), globals));
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
    output.object = unconvert(x, output);
    rds2cpp::write_rds(output, file_name);
    return R_NilValue;
}
