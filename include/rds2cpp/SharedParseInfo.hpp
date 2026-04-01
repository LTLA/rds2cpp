#ifndef RDS2CPP_SHARED_PARSE_HPP
#define RDS2CPP_SHARED_PARSE_HPP

#include <vector>
#include <memory>
#include <string>
#include <cstddef>
#include <stdexcept>

#include "RObject.hpp"
#include "SEXPType.hpp"
#include "utils_parse.hpp"

#include "sanisizer/sanisizer.hpp"

namespace rds2cpp {

struct SharedParseInfo {
    std::vector<Environment> environments;    

    std::vector<Symbol> symbols;

    std::vector<ExternalPointer> external_pointers;

    std::vector<std::pair<SEXPType, std::size_t> > mappings;
};

inline std::size_t extract_reference_index(const Header& header) {
    // Shouldn't matter that we use a signed integer here, as the left-shifts should never get to the sign bit.
    std::int32_t index = 0;
    for (int i = 0; i < 3; ++i) {
        index <<= 8;
        index += header[i];
    }
    if (index <= 0) {
        throw std::runtime_error("index of REFSXP is out of range");
    }
    return sanisizer::cast<std::size_t>(index - 1);
}

inline std::size_t check_reference_index(std::size_t i, SEXPType type, const std::string& value, const SharedParseInfo& shared) {
    if (i >= shared.mappings.size()) {
        throw std::runtime_error("index for REFSXP is out of range");
    }
    const auto& chosen = shared.mappings[i];
    if (chosen.first != type) {
        throw std::runtime_error("expected REFSXP to point to " + value);
    }
    return chosen.second;
}

// Don't return a reference to the Symbol itself, as 'symbols'
// could be resized at any time, possibly invalidating the reference.
inline std::size_t request_new_symbol(SharedParseInfo& shared) {
    const auto index = sanisizer::cast<std::size_t>(shared.symbols.size());
    shared.mappings.emplace_back(SEXPType::SYM, index);
    shared.symbols.emplace_back();
    return index;
}

inline std::size_t extract_symbol_index(const Header& header, const SharedParseInfo& shared) {
    const std::size_t i = extract_reference_index(header); 
    return check_reference_index(i, SEXPType::SYM, "a symbol", shared);
}

// Don't return a reference to the ExternalPointer itself, as 'external_pointerse'
// could be resized at any time, possibly invalidating the reference.
inline std::size_t request_new_external_pointer(SharedParseInfo& shared) {
    const auto index = sanisizer::cast<std::size_t>(shared.external_pointers.size());
    shared.mappings.emplace_back(SEXPType::EXTPTR, index);
    shared.external_pointers.emplace_back();
    return index;
}

// Don't return a reference to the Environment itself, as 'environments'
// could be resized at any time, possibly invalidating the reference.
inline std::size_t request_new_environment(SharedParseInfo& shared) {
    const auto index = sanisizer::cast<std::size_t>(shared.environments.size());
    shared.mappings.emplace_back(SEXPType::ENV, index);
    shared.environments.emplace_back();
    return index;
}

inline std::size_t extract_environment_index(const Header& header, const SharedParseInfo& shared) {
    const auto i = extract_reference_index(header); 
    return check_reference_index(i, SEXPType::ENV, "an environment", shared);
}

inline std::unique_ptr<RObject> resolve_reference(const Header& header, const SharedParseInfo& shared) {
    const auto index = extract_reference_index(header); 
    if (index >= shared.mappings.size()) {
        throw std::runtime_error("index for REFSXP is out of range");
    }
    const auto& chosen = shared.mappings[index];

    if (chosen.first == SEXPType::ENV) {
        return std::unique_ptr<RObject>(new EnvironmentIndex(chosen.second));
    } 

    if (chosen.first == SEXPType::SYM) {
        return std::unique_ptr<RObject>(new SymbolIndex(chosen.second));
    }

    return std::unique_ptr<RObject>(new ExternalPointerIndex(chosen.second));
} 

}

#endif
