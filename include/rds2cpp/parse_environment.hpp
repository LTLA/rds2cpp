#ifndef RDS2CPP_PARSE_ENVIRONMENT_HPP
#define RDS2CPP_PARSE_ENVIRONMENT_HPP

#include <cstdint>
#include <vector>
#include <algorithm>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

template<class Reader>
PairList parse_pairlist_body(Reader&, std::vector<unsigned char>&, SharedParseInfo&);

inline EnvironmentIndex parse_global_environment_body() {
    // Rely on default constructor.
    return EnvironmentIndex();
}

template<class Reader>
EnvironmentIndex parse_new_environment_body(Reader& reader, std::vector<unsigned char>& leftovers, SharedParseInfo& shared) try {
    // Need to provision the environment first, so that internal references are valid.
    size_t eindex = shared.request_environment();
    Environment new_env;

    // Is it locked or not?
    uint32_t locked = 0;
    extract_up_to(reader, leftovers, 4, [&](const unsigned char* buffer, size_t n, size_t) -> void {
        for (size_t j = 0; j < n; ++j) {
            locked <<= 8;
            locked += buffer[j];
        }        
    });
    new_env.locked = (locked > 0);

    // The next 4 bytes describe the parent environment.
    std::array<unsigned char, 4> parent;
    extract_up_to(reader, leftovers, 4, [&](const unsigned char* buffer, size_t n, size_t i) -> void {
        std::copy(buffer, buffer + n, parent.data() + i);
    });

    auto lastbit = parent[3];
    if (lastbit == static_cast<unsigned char>(SEXPType::REF)) {
        new_env.parent = shared.get_environment_index(parent);
        new_env.parent_type = SEXPType::ENV;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::ENV)) {
        auto env = parse_new_environment_body(reader, leftovers, shared);
        new_env.parent = env.index;
        new_env.parent_type = SEXPType::ENV;

    } else if (lastbit == static_cast<unsigned char>(SEXPType::GLOBALENV_)) {
        new_env.parent_type = SEXPType::GLOBALENV_;

    } else {
        throw std::runtime_error("could not resolve the parent environment");
    }

    auto unhashed = parse_header(reader, leftovers); 
    if (unhashed[3] == static_cast<unsigned char>(SEXPType::LIST)) {
        auto plist = parse_pairlist_body(reader, leftovers, unhashed, shared);

        for (size_t i = 0; i < plist.data.size(); ++i) {
            if (!plist.has_tag[i]) {
                throw std::runtime_error("unhashed environment values should be respresented in a tagged pairlist");
            }
            new_env.variable_values.emplace_back(std::move(plist.data[i]));
            new_env.variable_names.emplace_back(std::move(plist.tag_names[i]));
            new_env.variable_encodings.emplace_back(plist.tag_encodings[i]);
        }

        auto hashed = parse_header(reader, leftovers); 
        if (hashed[3] != static_cast<unsigned char>(SEXPType::NILVALUE_)) {
            throw std::runtime_error("unhashed environment should not contain a non-NULL hash table");
        }

    } else if (unhashed[3] == static_cast<unsigned char>(SEXPType::NILVALUE_)) {
        // The next part is the hash table.
        auto hash_header = parse_header(reader, leftovers);
        if (hash_header[3] != static_cast<unsigned char>(SEXPType::VEC)) {
            throw std::runtime_error("environment's hash table should be a list");
        }
        new_env.hashed = true;

        auto vec = parse_list_body(reader, leftovers, shared);
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
    auto attr_header = parse_header(reader, leftovers); 
    if (attr_header[3] == static_cast<unsigned>(SEXPType::LIST)) {
        parse_attributes_body(reader, leftovers, attr_header, new_env.attributes, shared);
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
