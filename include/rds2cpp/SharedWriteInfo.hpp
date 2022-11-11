#ifndef RDS2CPP_SHARED_WRITE_HPP
#define RDS2CPP_SHARED_WRITE_HPP

#include <vector>
#include <memory>
#include <string>

#include "Environment.hpp"
#include "Symbol.hpp"
#include "RObject.hpp"
#include "SEXPType.hpp"

#include "write_single_string.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

struct SharedWriteInfo {
    size_t reference_count;

    std::vector<std::unordered_map<std::string, size_t> > symbol_mappings;

    const std::vector<Symbol>* known_symbols;

    std::vector<size_t> known_symbol_mappings;

    const std::vector<Environment>* known_environments;

public:
    SharedWriteInfo(const std::vector<Symbol>& s, const std::vector<Environment>& e) : 
        reference_count(1), 
        symbol_mappings(static_cast<int>(StringEncoding::ASCII) + 1),
        known_symbols(&s),
        known_symbol_mappings(s.size())
    {}

private:
    template<class Writer>
    static void write_reference(size_t ref, Writer& writer, std::vector<unsigned char>& buffer) {
        buffer.resize(4);
        buffer[2] = ref & 255;
        ref >>= 8;
        buffer[1] = ref & 255;
        ref >>= 8;
        buffer[0] = ref & 255;
        buffer[3] = static_cast<unsigned char>(SEXPType::REF);
        writer.write(buffer.data(), buffer.size());
        return;
    }

public:
    template<class Writer>
    size_t write_symbol(const std::string& value, StringEncoding encoding, Writer& writer, std::vector<unsigned char>& buffer) {
        auto& host = symbol_mappings[static_cast<int>(encoding)];
        auto it = host.find(value);
        if (it != host.end()) {
            write_reference(it->second, writer, buffer);
            return it->second;
        }

        buffer.clear();
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(0);
        buffer.push_back(static_cast<unsigned char>(SEXPType::SYM));

        writer.write(buffer.data(), buffer.size());
        write_single_string(value, encoding, false, writer, buffer);

        host[value] = reference_count;
        return reference_count++;
    }

    template<class Writer>
    void write_symbol(size_t index, Writer& writer, std::vector<unsigned char>& buffer) {
        if (index >= known_symbol_mappings.size()) {
            throw std::runtime_error("symbol index out of range of supplied Symbol objects");
        }

        auto& candidate = known_symbol_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, writer, buffer);
        } else {
            const auto& sym = (*known_symbols)[index];
            known_symbol_mappings[index] = write_symbol(sym.name, sym.encoding, writer, buffer);
        }
    }
};

}

#endif
