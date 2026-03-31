#ifndef RDS2CPP_SHARED_WRITE_HPP
#define RDS2CPP_SHARED_WRITE_HPP

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "RObject.hpp"
#include "SEXPType.hpp"
#include "write_single_string.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

struct SharedWriteInfo;

template<class BufferedWriter_>
void write_object(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared);

struct SharedWriteInfo {
    std::size_t reference_count;

    std::vector<std::unordered_map<std::string, std::size_t> > symbol_mappings;

    const std::vector<Symbol>* known_symbols;

    const std::vector<Environment>* known_environments;

    const std::vector<ExternalPointer>* known_external_pointers;

    std::vector<std::size_t> known_symbol_mappings, known_environment_mappings, known_external_pointer_mappings;

public:
    SharedWriteInfo(
        const std::vector<Symbol>& s, 
        const std::vector<Environment>& e, 
        const std::vector<ExternalPointer>& ex
    ) :
        reference_count(1), 
        symbol_mappings(sanisizer::sum<I<decltype(symbol_mappings.size())> >(static_cast<int>(StringEncoding::ASCII), 1)),
        known_symbols(&s),
        known_environments(&e),
        known_external_pointers(&ex),
        known_symbol_mappings(sanisizer::cast<I<decltype(known_symbol_mappings.size())> >(s.size())),
        known_environment_mappings(sanisizer::cast<I<decltype(known_environment_mappings.size())> >(e.size())),
        known_external_pointer_mappings(sanisizer::cast<I<decltype(known_external_pointer_mappings.size())> >(ex.size()))
    {}

private:
    template<class BufferedWriter_>
    static void write_reference(std::size_t ref, BufferedWriter_& bufwriter) {
        Header details;

        // Opposite of SharedParseInfo::compute_reference_index.
        for (int i = 0; i < 3; ++i) {
            details[2 - i] = ref & 255;
            ref >>= 8;
        }

        details[3] = static_cast<unsigned char>(SEXPType::REF);
        bufwriter.write(details.data(), details.size());
    }

public:
    template<class BufferedWriter_>
    std::size_t write_symbol(const std::string& value, StringEncoding encoding, BufferedWriter_& bufwriter) {
        auto& host = symbol_mappings[static_cast<int>(encoding)];
        auto it = host.find(value);
        if (it != host.end()) {
            write_reference(it->second, bufwriter);
            return it->second;
        }

        inject_header(SEXPType::SYM, bufwriter);
        write_single_string(value, encoding, bufwriter);

        const auto old_reference_count = reference_count;
        host[value] = reference_count;
        reference_count = sanisizer::sum<I<decltype(reference_count)> >(reference_count, 1); // safely incrementing this count.

        return old_reference_count;
    }

    template<class BufferedWriter_>
    void write_symbol(const RObject* obj, BufferedWriter_& bufwriter) {
        auto ptr = static_cast<const SymbolIndex*>(obj);
        const auto index = ptr->index;
        if (index >= known_symbol_mappings.size()) {
            throw std::runtime_error("symbol index out of range for supplied Symbol objects");
        }

        auto& candidate = known_symbol_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, bufwriter);
        } else {
            const auto& sym = (*known_symbols)[index];
            candidate = write_symbol(sym.name, sym.encoding, bufwriter);
        }
    }

public:
    template<class BufferedWriter_>
    void write_external_pointer(const RObject* obj, BufferedWriter_& bufwriter) {
        auto ptr = static_cast<const ExternalPointerIndex*>(obj);
        auto index = ptr->index;
        if (index >= known_external_pointer_mappings.size()) {
            throw std::runtime_error("external pointer index out of range for supplied ExternalPointer objects");
        }

        auto& candidate = known_external_pointer_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, bufwriter);
            return;
        }
        candidate = reference_count;
        reference_count = sanisizer::sum<I<decltype(reference_count)> >(reference_count, 1); // safely incrementing this count.

        const auto& ext = (*known_external_pointers)[index];
        inject_header(SEXPType::EXTPTR, ext.attributes, bufwriter, *this);
        write_object(ext.protection.get(), bufwriter, *this);
        write_object(ext.tag.get(), bufwriter, *this);
        write_attributes(ext.attributes, bufwriter, *this);
    }

public:
    template<class BufferedWriter_>
    void write_environment(const RObject* obj, BufferedWriter_& bufwriter) {
        auto ptr = static_cast<const EnvironmentIndex*>(obj);
        auto index = ptr->index;
        auto env_type = ptr->env_type;

        if (env_type == SEXPType::GLOBALENV_ || env_type == SEXPType::BASEENV_ || env_type == SEXPType::EMPTYENV_) {
            inject_header(env_type, bufwriter);
            return;
        }

        if (index >= known_environment_mappings.size()) {
            throw std::runtime_error("environment index out of range for supplied Environment objects");
        }

        auto& candidate = known_environment_mappings[index];
        if (candidate != 0) {
            write_reference(candidate, bufwriter);
            return;
        }
        candidate = reference_count++;

        const auto& env = (*known_environments)[index];
        inject_header(SEXPType::ENV, env.attributes, bufwriter, *this);
        inject_integer<std::int32_t, std::int32_t>(env.locked, bufwriter);

        {
            EnvironmentIndex parent;
            parent.index = env.parent;
            parent.env_type = env.parent_type;
            write_environment(&parent, bufwriter);
        }

        // Creating a tagged pairlist per element.
        for (const auto& var : env.variables) {
            inject_next_pairlist_header(true, bufwriter);
            write_symbol(&(var.name), bufwriter);
            write_object(var.value.get(), bufwriter, *this);
        }
        inject_header(SEXPType::NILVALUE_, bufwriter);

        // We're not saving a hash table, because I don't want to have to
        // reproduce R's environment hashing logic.
        inject_header(SEXPType::NILVALUE_, bufwriter);

        // Forcibly writing out all attributes with NIL termination.
        write_attributes_body(env.attributes, bufwriter, *this);
    }
};

}

#endif
