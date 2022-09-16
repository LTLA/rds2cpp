#ifndef RDS2CPP_SHARED_HPP
#define RDS2CPP_SHARED_HPP

#include <vector>
#include <memory>
#include <string>

#include "StringEncoding.hpp"
#include "RObject.hpp"

namespace rds2cpp {

struct Environment {
    bool locked = false;

    SEXPType parent_type = SEXPType::ENV;
    size_t parent = -1;

    std::vector<std::string> variable_names;
    std::vector<StringEncoding> variable_encodings;
    std::vector<std::unique_ptr<RObject> > variable_values;

    std::vector<std::string> attribute_names;
    std::vector<StringEncoding> attribute_encodings;
    std::vector<std::unique_ptr<RObject> > attribute_values;
};

struct Symbol {
    std::string name;
    StringEncoding encoding;
};

struct Shared {
    std::vector<Environment> environments;    
    std::vector<Symbol> symbols;
    std::vector<std::pair<SEXPType, size_t> > mappings;

private:
    size_t compute_reference_index(const Header& header) const {
        size_t index = 0;
        for (int i = 0; i < 3; ++i) {
            index <<= 8;
            index += header[i];
        }

        if (index == 0 || index > mappings.size()) {
            throw std::runtime_error("index of REFSXP is out of range");
        }
        return index - 1;
    }

    size_t check_environment_index(size_t i) const {
        if (i >= mappings.size()) {
            throw std::runtime_error("index for REFSXP is out of range");
        }
        const auto& chosen = mappings[i];
        if (chosen.first != SEXPType::ENV) {
            throw std::runtime_error("expected REFSXP to point to an environment");
        }
        return chosen.second;
    }

    size_t check_symbol_index(size_t i) const {
        if (i >= mappings.size()) {
            throw std::runtime_error("index for REFSXP is out of range");
        }
        const auto& chosen = mappings[i];
        if (chosen.first != SEXPType::SYM) {
            throw std::runtime_error("expected REFSXP to point to a symbol");
        }
        return chosen.second;
    }

public:
    size_t add_symbol(Symbol s) {
        size_t index = symbols.size();
        mappings.emplace_back(SEXPType::SYM, index);
        symbols.emplace_back(std::move(s));
        return index;
    }

    const Symbol& get_symbol(size_t i) const {
        auto j = check_symbol_index(i);
        return symbols[j];
    }

    size_t get_symbol_index(const Header& header) const {
        size_t i = compute_reference_index(header); 
        return check_symbol_index(i);
    }

    const Symbol& get_symbol(const Header& header) const {
        size_t j = get_symbol_index(header);
        return symbols[j];
    }

public:
    // Don't return a reference to the Environment itself, as 'environments'
    // could be resized at any time, possibly invalidating the reference.
    size_t request_environment() {
        size_t index = environments.size();
        mappings.emplace_back(SEXPType::ENV, index);
        environments.resize(index + 1);
        return index;
    }

    const Environment& get_environment(size_t i) const {
        auto j = check_environment_index(i);
        return environments[j];
    }

    size_t get_environment_index(const Header& header) const {
        size_t i = compute_reference_index(header); 
        return check_environment_index(i);
    }

    const Environment& get_environment(const Header& header) const {
        auto j = get_environment_index(header);
        return environments[j];
    }

public:
    std::unique_ptr<RObject> resolve_reference(const Header& header) const {
        size_t index = compute_reference_index(header); 
        if (index >= mappings.size()) {
            throw std::runtime_error("index for REFSXP is out of range");
        }
        auto chosen = mappings[index];

        if (chosen.first == SEXPType::ENV) {
            EnvironmentIndex output;
            output.type = SEXPType::ENV;
            output.index = chosen.second;
            return std::unique_ptr<RObject>(new EnvironmentIndex(std::move(output)));
        } 

        SymbolIndex output;
        output.index = chosen.second;
        return std::unique_ptr<RObject>(new SymbolIndex(std::move(output)));
    } 
};

}

#endif
