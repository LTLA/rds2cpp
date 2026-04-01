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
std::unique_ptr<PairList> parse_pairlist_body(Source_&, SharedParseInfo&);

inline std::unique_ptr<EnvironmentIndex> parse_global_environment_body() {
    return std::make_unique<EnvironmentIndex>(SEXPType::GLOBALENV_);
}

inline std::unique_ptr<EnvironmentIndex> parse_base_environment_body() {
    return std::make_unique<EnvironmentIndex>(SEXPType::BASEENV_);
}

inline std::unique_ptr<EnvironmentIndex> parse_empty_environment_body() {
    return std::make_unique<EnvironmentIndex>(SEXPType::EMPTYENV_);
}

template<class Source_>
std::unique_ptr<EnvironmentIndex> parse_new_environment_body(Source_& src, SharedParseInfo& shared) try {
    // Need to provision the environment first, so that internal references are valid.
    // Don't create an lvalue reference to 'shared.environments[eindex]',
    // as this might be invalidated by reallocations to 'shared.environments'.
    const auto eindex = request_new_environment(shared);
    Environment new_env;

    // Is it locked or not?
    const auto locked = quick_integer<std::int32_t>(src);
    new_env.locked = (locked > 0);

    // The next 4 bytes describe the parent environment.
    std::array<unsigned char, 4> parent;
    quick_extract(src, parent.size(), parent.data());

    auto lastbit = parent[3];
    if (lastbit == static_cast<unsigned char>(SEXPType::REF)) {
        new_env.parent = extract_environment_index(parent, shared);
        new_env.parent_type = SEXPType::ENV;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::ENV)) {
        auto env = parse_new_environment_body(src, shared);
        new_env.parent = env->index;
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

        new_env.variables.reserve(plist->data.size());
        for (auto& entry : plist->data) {
            if (!(entry.tag.has_value())) {
                throw std::runtime_error("unhashed environment values should be respresented in a tagged pairlist");
            }
            new_env.variables.emplace_back(std::move(*(entry.tag)), std::move(entry.value));
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
        for (auto& sublist : vec->data) {
            const auto subtype = sublist->type();
            if (subtype == SEXPType::NIL) {
                continue;
            }
            if (subtype != SEXPType::LIST) {
                throw std::runtime_error("environment values should be represented as pairlists");
            }

            auto plist = static_cast<PairList*>(sublist.get());
            new_env.variables.reserve(new_env.variables.size() + plist->data.size());

            for (auto& entry : plist->data) {
                if (!entry.tag.has_value()) {
                    throw std::runtime_error("environment values should be stored in a tagged pairlist");
                }
                new_env.variables.emplace_back(std::move(*(entry.tag)), std::move(entry.value));
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
    return std::make_unique<EnvironmentIndex>(eindex);
} catch (std::exception& e) {
    throw traceback("failed to parse a new environment body", e);
    return std::unique_ptr<EnvironmentIndex>();
}

}

#endif
