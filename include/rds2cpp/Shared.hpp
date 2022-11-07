#ifndef RDS2CPP_SHARED_HPP
#define RDS2CPP_SHARED_HPP

#include <vector>
#include <memory>
#include <string>

#include "StringEncoding.hpp"
#include "RObject.hpp"
#include "utils.hpp"

/**
 * @file Shared.hpp
 *
 * @brief Shared values that are re-used within an unserialized object.
 */

namespace rds2cpp {

/**
 * @brief An R environment.
 */
struct Environment {
    /**
     * Whether the environment was locked.
     */
    bool locked = false;

    /**
     * Type of the parent environment.
     */
    SEXPType parent_type = SEXPType::ENV;

    /** 
     * Index of the parent environment.
     * This should only be used if `paret_type` is `ENV`.
     */
    size_t parent = -1;

    /**
     * Names of the variables inside this environment.
     */
    std::vector<std::string> variable_names;

    /**
     * Encodings of the variable names in `variable_names`.
     */
    std::vector<StringEncoding> variable_encodings;

    /**
     * Values of the variables in this environment.
     */
    std::vector<std::unique_ptr<RObject> > variable_values;

    /**
     * Additional attributes.
     */
    Attributes attributes;
};

/**
 * @brief An R symbol.
 */
struct Symbol {
    /**
     * Name of the symbol.
     */
    std::string name;

    /**
     * Encoding for the symbol name.
     */
    StringEncoding encoding;
};

/**
 * @cond
 */
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
            output.index = chosen.second;
            return std::unique_ptr<RObject>(new EnvironmentIndex(std::move(output)));
        } 

        SymbolIndex output;
        output.index = chosen.second;
        return std::unique_ptr<RObject>(new SymbolIndex(std::move(output)));
    } 
};
/**
 * @endcond
 */

}

#endif
