#ifndef RDS2CPP_PARSE_ENVIRONMENT_HPP
#define RDS2CPP_PARSE_ENVIRONMENT_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

template<class Source_>
PairList parse_pairlist_body(Source_&, SharedParseInfo&);

inline EnvironmentIndex parse_global_environment_body() {
    return EnvironmentIndex(SEXPType::GLOBALENV_);
}

inline EnvironmentIndex parse_base_environment_body() {
    return EnvironmentIndex(SEXPType::BASEENV_);
}

inline EnvironmentIndex parse_empty_environment_body() {
    return EnvironmentIndex(SEXPType::EMPTYENV_);
}

template<class Source_>
EnvironmentIndex parse_new_environment_body(Source_& src, SharedParseInfo& shared) try {
    // Need to provision the environment first, so that internal references are valid.
    size_t eindex = shared.request_environment();
    Environment new_env;

    // Is it locked or not?
    uint32_t locked = 0;
    for (int i = 0; i < 4; ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        locked <<= 8;
        locked += src.get();
    }
    new_env.locked = (locked > 0);

    // The next 4 bytes describe the parent environment.
    std::array<unsigned char, 4> parent;
    for (int i = 0; i < 4; ++i) {
        if (!src.advance()) {
            throw empty_error();
        }
        parent[i] = src.get();
    }

    auto lastbit = parent[3];
    if (lastbit == static_cast<unsigned char>(SEXPType::REF)) {
        new_env.parent = shared.get_environment_index(parent);
        new_env.parent_type = SEXPType::ENV;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::ENV)) {
        auto env = parse_new_environment_body(src, shared);
        new_env.parent = env.index;
        new_env.parent_type = SEXPType::ENV;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::GLOBALENV_)) {
        new_env.parent_type = SEXPType::GLOBALENV_;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::BASEENV_)) {
        new_env.parent_type = SEXPType::BASEENV_;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::NILVALUE_)) { // handle weirdness when unserializing igraph graph objects... oh well.
        new_env.parent_type = SEXPType::BASEENV_;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::EMPTYENV_)) { 
        new_env.parent_type = SEXPType::EMPTYENV_;

    } else {
        throw std::runtime_error("could not resolve the parent environment (" + std::to_string(lastbit) + ")");
    }

    auto unhashed = parse_header(src); 
    if (unhashed[3] == static_cast<unsigned char>(SEXPType::LIST)) {
        auto plist = parse_pairlist_body(src, unhashed, shared);

        for (size_t i = 0; i < plist.data.size(); ++i) {
            if (!plist.has_tag[i]) {
                throw std::runtime_error("unhashed environment values should be respresented in a tagged pairlist");
            }
            new_env.variable_values.emplace_back(std::move(plist.data[i]));
            new_env.variable_names.emplace_back(std::move(plist.tag_names[i]));
            new_env.variable_encodings.emplace_back(plist.tag_encodings[i]);
        }

        auto hashed = parse_header(src); 
        if (hashed[3] != static_cast<unsigned char>(SEXPType::NILVALUE_)) {
            throw std::runtime_error("unhashed environment should not contain a non-NULL hash table");
        }

    } else if (unhashed[3] == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        // The next part is the hash table.
        auto hash_header = parse_header(src);
        if (hash_header[3] != static_cast<unsigned char>(SEXPType::VEC)) {
            throw std::runtime_error("environment's hash table should be a list");
        }
        new_env.hashed = true;

        auto vec = parse_list_body(src, shared);
        for (size_t i = 0; i < vec.data.size(); ++i) {
            if (vec.data[i]->type() == SEXPType::NIL) {
                continue;
            }

            if (vec.data[i]->type() != SEXPType::LIST) {
                throw std::runtime_error("environment values should be represented as pairlists");
            }

            auto plist = static_cast<PairList*>(vec.data[i].get());
            auto nelements = plist->data.size();
            for (size_t j = 0; j < nelements; ++j) {
                if (!plist->has_tag[j]) {
                    throw std::runtime_error("environment values should be represented as a length-1 tagged pairlists");
                }
                new_env.variable_values.emplace_back(std::move(plist->data[j]));
                new_env.variable_names.emplace_back(std::move(plist->tag_names[j]));
                new_env.variable_encodings.emplace_back(plist->tag_encodings[j]);
            }
        }

    } else {
        throw std::runtime_error("environment's parent and hash table should be separated by a null");
    } 

    // Attributes or NULL.
    auto attr_header = parse_header(src); 
    if (attr_header[3] == static_cast<unsigned>(SEXPType::LIST)) {
        parse_attributes_body(src, attr_header, new_env.attributes, shared);
    } else if (attr_header[3] != static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        throw std::runtime_error("environment should be terminated by a null");
    }

    shared.environments[eindex] = std::move(new_env);
    return EnvironmentIndex(eindex);
} catch (std::exception& e) {
    throw traceback("failed to parse a new environment body", e);
}

}

#endif
