#ifndef RDS2CPP_PARSE_ENVIRONMENT_HPP
#define RDS2CPP_PARSE_ENVIRONMENT_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "utils.hpp"

namespace rds2cpp {

namespace environment_internals {

void find_parent_env(const Header& header, const Globals& globals, size_t& index) {
    index = 0;
    for (int i = 0; i < 3; ++i) {
        index << 8;
        index += parent[i];
    }
    if (index >= globals.environments.size()) {
        throw std::runtime_error("index of existing environment is out of range");
    }
}

}

template<class Reader>
Environment parse_global_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, Globals& globals) {
    EnvironmentIndex output;
    globals.environments.emplace_back(nullptr);
    output.global = true;
    return output;
}

template<class Reader>
Environment parse_existing_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, Globals& globals) {
    EnvironmentIndex output;
    output.global = false;
    environment_internals::find_parent_env(header, globals, output.index);
    return output;
}

template<class Reader>
Environment parse_new_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, Globals& globals) {
    EnvironmentIndex output;
    Environment new_env;

    // Mystery 4 bytes here.
    extract_up_to(reader, leftovers, 4, [&](const unsigned char*, size_t, size_t) -> void {});

    // The next 4 bytes describe the parent environment.
    std::array<unsigned char, 4> parent;
    extract_up_to(reader, leftovers, 4, [&](const unsigned char* buffer, size_t n, size_t i) -> void {
        std::copy(buffer, buffer + n, parent.data() + i);
    });

    auto lastbit = static_cast<unsigned char>(parent[4]);
    if (lastbit == 255) {
        environment_internals::find_parent_env(header, globals, new_env.parent);

    } else if (lastbit == 4) {
        auto env = parse_environment_body(reader, leftovers, parent, globals);
        new_env.parent = env.index;

    } else {
        throw std::runtime_error("could not resolve the parent environment");
    }

    // Who knows what this is...
    auto header = extract_header(reader, leftovers); 
    if (header[3] != 254) {
        throw std::runtime_error("environment's parent and hash table should be separated by a null");
    }

    // The next part is the hash table.
    auto hash_header = parse_header(reader, leftovers);
    if (hash_header[3] != static_cast<unsigned char>(VEC)) {
        throw std::runtime_error("environment's hash table should be a list");
    }

    auto vec = parse_list_body(reader, leftovers);
    for (size_t i = 0; i < vec.data.size(); ++i) {
        if (vec.data[i]->sexp_type == NIL) {
            continue;
        }

        if (vec.data[i]->sexp_type != LIST) {
            throw std::runtime_error("environment values should be represented as pairlists");
        }

        auto plist = static_cast<List*>(vec.data[i].get());
        if (plist->data.size() != 1 || !plist->has_tag[0]) {
            throw std::runtime_error("environment values should be represented as a length-1 tagged pairlists");
        }

        new_env.variable_values.emplace_back(std::move(plist->data[i]));
        new_env.variable_names.emplace_back(std::move(plist->tag_names[i]));
        new_env.variable_encodings.emplace_back(plist->tag_encodings[i]);
    }

    // Who knows what this is...
    auto header = extract_header(reader, leftovers); 
    if (header[3] != 254) {
        throw std::runtime_error("environment should be terminated by a null");
    }

    output.index = globals.environments.size();
    output.global = false;
    globals.environments.emplace_back(std::move(new_env));

    return output;
};

}

#endif
