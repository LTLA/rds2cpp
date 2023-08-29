#ifndef RDS2CPP_SHARED_WRITE_HPP
#define RDS2CPP_SHARED_WRITE_HPP

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "Environment.hpp"
#include "Symbol.hpp"
#include "ExternalPointer.hpp"
#include "RObject.hpp"
#include "SEXPType.hpp"

#include "write_single_string.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

struct SharedWriteInfo;

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared);

struct SharedWriteInfo {
    size_t reference_count;

    std::vector<std::unordered_map<std::string, size_t> > symbol_mappings;

    const std::vector<Symbol>* known_symbols;

    const std::vector<Environment>* known_environments;

    const std::vector<ExternalPointer>* known_external_pointers;

    std::vector<size_t> known_symbol_mappings, known_environment_mappings, known_external_pointer_mappings;

public:
    SharedWriteInfo(
        const std::vector<Symbol>& s, 
        const std::vector<Environment>& e, 
        const std::vector<ExternalPointer>& ex
    ) :
        reference_count(1), 
        symbol_mappings(static_cast<int>(StringEncoding::ASCII) + 1),
        known_symbols(&s),
        known_environments(&e),
        known_external_pointers(&ex),
        known_symbol_mappings(s.size()),
        known_environment_mappings(e.size()),
        known_external_pointer_mappings(ex.size())
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
        inject_header(SEXPType::SYM, buffer);
        writer.write(buffer.data(), buffer.size());

        write_single_string(value, encoding, false, writer, buffer);

        host[value] = reference_count;
        return reference_count++;
    }

    template<class Writer>
    void write_symbol(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
        auto ptr = static_cast<const SymbolIndex*>(obj);
        auto index = ptr->index;
        if (index >= known_symbol_mappings.size()) {
            throw std::runtime_error("symbol index out of range for supplied Symbol objects");
        }

        auto& candidate = known_symbol_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, writer, buffer);
        } else {
            const auto& sym = (*known_symbols)[index];
            candidate = write_symbol(sym.name, sym.encoding, writer, buffer);
        }
    }

public:
    template<class Writer>
    void write_external_pointer(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
        auto ptr = static_cast<const ExternalPointerIndex*>(obj);
        auto index = ptr->index;
        if (index >= known_external_pointer_mappings.size()) {
            throw std::runtime_error("external pointer index out of range for supplied ExternalPointer objects");
        }

        auto& candidate = known_external_pointer_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, writer, buffer);
            return;
        }
        candidate = reference_count++;

        const auto& ext = (*known_external_pointers)[index];

        buffer.clear();
        inject_header(SEXPType::EXTPTR, ext.attributes, buffer);
        writer.write(buffer.data(), buffer.size());

        write_object(ext.protection.get(), writer, buffer, *this);
        write_object(ext.tag.get(), writer, buffer, *this);
        write_attributes(ext.attributes, writer, buffer, *this);

        return;
    }

public:
    template<class Writer>
    void write_environment(const RObject* obj, Writer& writer, std::vector<unsigned char>& buffer) {
        auto ptr = static_cast<const EnvironmentIndex*>(obj);
        auto index = ptr->index;
        auto env_type = ptr->env_type;

        if (env_type == SEXPType::GLOBALENV_ || env_type == SEXPType::BASEENV_ || env_type == SEXPType::EMPTYENV_) {
            buffer.clear();
            inject_header(env_type, buffer);
            writer.write(buffer.data(), buffer.size());
            return;
        }

        if (index >= known_environment_mappings.size()) {
            throw std::runtime_error("environment index out of range for supplied Environment objects");
        }

        auto& candidate = known_environment_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, writer, buffer);
            return;
        }

        candidate = reference_count++;
        const auto& env = (*known_environments)[index];

        buffer.clear();
        inject_header(SEXPType::ENV, env.attributes, buffer);

        buffer.insert(buffer.end(), 3, 0);
        buffer.push_back(env.locked);
        writer.write(buffer.data(), buffer.size());

        {
            EnvironmentIndex parent;
            parent.index = env.parent;
            parent.env_type = env.parent_type;
            write_environment(&parent, writer, buffer);
        }

        const auto& names = env.variable_names;
        const auto& encodings = env.variable_encodings;
        const auto& values = env.variable_values;

        size_t len = names.size();
        if (len) {
            // Creating a tagged pairlist per element.
            for (size_t i = 0; i < len; ++i) {
                buffer.clear();
                inject_next_pairlist_header(true, buffer);
                writer.write(buffer.data(), buffer.size());

                write_symbol(names[i], encodings[i], writer, buffer);
                write_object(values[i].get(), writer, buffer, *this);
            }
        }

        // Terminating the pairlist.
        buffer.clear();
        inject_header(SEXPType::NILVALUE_, buffer);
        writer.write(buffer.data(), buffer.size());

        // We're not saving a hash table, because I don't want to have to
        // reproduce R's environment hashing logic.
        buffer.clear();
        inject_header(SEXPType::NILVALUE_, buffer);
        writer.write(buffer.data(), buffer.size());

        if (!write_attributes(env.attributes, writer, buffer, *this)) {
            // Finishing with NULL if there aren't any attributes.
            buffer.clear();
            inject_header(SEXPType::NILVALUE_, buffer);
            writer.write(buffer.data(), buffer.size());
        }
    }
};

}

#endif
