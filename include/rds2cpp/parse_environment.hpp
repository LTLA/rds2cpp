#ifndef RDS2CPP_PARSE_ENVIRONMENT_HPP
#define RDS2CPP_PARSE_ENVIRONMENT_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "Shared.hpp"
#include "utils.hpp"

namespace rds2cpp {

template<class Reader>
PairList parse_pairlist_body(Reader&, std::vector<unsigned char>&, Shared&);

namespace environment_internals {

size_t find_parent_env(const Header& header, const Shared& shared) {
    size_t index = 0;
    for (int i = 0; i < 3; ++i) {
        index <<= 8;
        index += header[i];
    }

    if (index == 0 || index > shared.environment_mappings.size()) {
        throw std::runtime_error("index of existing environment is out of range");
    }

    auto mapped = shared.environment_mappings[index - 1];
    if (mapped == -1) {
        throw std::runtime_error("index of existing environment refers to the global environment");
    }

    return mapped;
}

}

template<class Reader>
EnvironmentIndex parse_global_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, Shared& shared) {
    EnvironmentIndex output;
    shared.environment_mappings.push_back(-1);
    output.index = -1;
    output.global = true;
    return output;
}

template<class Reader>
EnvironmentIndex parse_existing_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, Shared& shared) {
    EnvironmentIndex output;
    output.index = environment_internals::find_parent_env(header, shared);
    output.global = false;
    return output;
}

template<class Reader>
EnvironmentIndex parse_new_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, Shared& shared) {
    EnvironmentIndex output;
    Environment new_env;

    // Mystery 4 bytes here.
    extract_up_to(reader, leftovers, 4, [&](const unsigned char*, size_t, size_t) -> void {});

    // The next 4 bytes describe the parent environment.
    std::array<unsigned char, 4> parent;
    extract_up_to(reader, leftovers, 4, [&](const unsigned char* buffer, size_t n, size_t i) -> void {
        std::copy(buffer, buffer + n, parent.data() + i);
    });

    auto lastbit = static_cast<unsigned char>(parent[3]);
    if (lastbit == 255) {
        new_env.parent = environment_internals::find_parent_env(parent, shared);

    } else if (lastbit == 4) {
        auto env = parse_new_environment_body(reader, leftovers, parent, shared);
        new_env.parent = env.index;

    } else if (lastbit == 253) {
        new_env.parent = -1; // i.e., global env is the parent.

    } else {
        throw std::runtime_error("could not resolve the parent environment");
    }

    // Who knows what this is...
    auto mystery1 = parse_header(reader, leftovers); 
    if (mystery1[3] != 254) {
        throw std::runtime_error("environment's parent and hash table should be separated by a null");
    }

    // The next part is the hash table.
    auto hash_header = parse_header(reader, leftovers);
    if (hash_header[3] != static_cast<unsigned char>(SEXPType::VEC)) {
        throw std::runtime_error("environment's hash table should be a list");
    }

    auto vec = parse_list_body(reader, leftovers, shared);
    for (size_t i = 0; i < vec.data.size(); ++i) {
        if (vec.data[i]->sexp_type == SEXPType::NIL) {
            continue;
        }

        if (vec.data[i]->sexp_type != SEXPType::LIST) {
            throw std::runtime_error("environment values should be represented as pairlists");
        }

        auto plist = static_cast<PairList*>(vec.data[i].get());
        if (plist->data.size() != 1 || !plist->has_tag[0]) {
            throw std::runtime_error("environment values should be represented as a length-1 tagged pairlists");
        }

        new_env.variable_values.emplace_back(std::move(plist->data[0]));
        new_env.variable_names.emplace_back(std::move(plist->tag_names[0]));
        new_env.variable_encodings.emplace_back(plist->tag_encodings[0]);
    }

    // Who knows what this is... a terminator for the environment, I guess?
    auto mystery2 = parse_header(reader, leftovers); 
    if (mystery2[3] != 254) {
        throw std::runtime_error("environment should be terminated by a null");
    }

    output.index = shared.environments.size();
    output.global = false;
    shared.environments.emplace_back(std::move(new_env));
    shared.environment_mappings.push_back(output.index);

    return output;
};

}

#endif
