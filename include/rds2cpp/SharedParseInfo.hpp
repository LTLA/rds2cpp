#ifndef RDS2CPP_SHARED_PARSE_HPP
#define RDS2CPP_SHARED_PARSE_HPP

#include <vector>
#include <memory>
#include <string>

#include "Environment.hpp"
#include "Symbol.hpp"
#include "ExternalPointer.hpp"
#include "RObject.hpp"
#include "SEXPType.hpp"
#include "utils_parse.hpp"

namespace rds2cpp {

struct SharedParseInfo {
    std::vector<Environment> environments;    

    std::vector<Symbol> symbols;

    std::vector<ExternalPointer> external_pointers;

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

    size_t check_reference_index(size_t i, SEXPType type, const std::string& value) const {
        if (i >= mappings.size()) {
            throw std::runtime_error("index for REFSXP is out of range");
        }
        const auto& chosen = mappings[i];
        if (chosen.first != type) {
            throw std::runtime_error("expected REFSXP to point to " + value);
        }
        return chosen.second;
    }

public:
    size_t request_symbol() {
        size_t index = symbols.size();
        mappings.emplace_back(SEXPType::SYM, index);
        symbols.resize(index + 1);
        return index;
    }

    size_t get_symbol_index(const Header& header) const {
        size_t i = compute_reference_index(header); 
        return check_reference_index(i, SEXPType::SYM, "a symbol");
    }

public:
    size_t request_external_pointer() {
        size_t index = external_pointers.size();
        mappings.emplace_back(SEXPType::EXTPTR, index);
        external_pointers.resize(index + 1);
        return index;
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

    size_t get_environment_index(const Header& header) const {
        size_t i = compute_reference_index(header); 
        return check_reference_index(i, SEXPType::ENV, "an environment");
    }

public:
    std::unique_ptr<RObject> resolve_reference(const Header& header) const {
        size_t index = compute_reference_index(header); 
        if (index >= mappings.size()) {
            throw std::runtime_error("index for REFSXP is out of range");
        }
        auto chosen = mappings[index];

        if (chosen.first == SEXPType::ENV) {
            return std::unique_ptr<RObject>(new EnvironmentIndex(chosen.second));
        } 

        if (chosen.first == SEXPType::SYM) {
            return std::unique_ptr<RObject>(new SymbolIndex(chosen.second));
        }

        return std::unique_ptr<RObject>(new ExternalPointerIndex(chosen.second));
    } 
};

}

#endif
