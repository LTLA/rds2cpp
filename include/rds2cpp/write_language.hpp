#ifndef RDS2CPP_WRITE_LANGUAGE_HPP
#define RDS2CPP_WRITE_LANGUAGE_HPP

#include <vector>
#include <memory>
#include <stdexcept>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

template<class Writer>
void write_object(const RObject*, Writer&, std::vector<unsigned char>&, SharedWriteInfo&);

template<class Writer>
void write_language(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    const LanguageObject& input = *static_cast<const LanguageObject*>(object);

    // Just another pairlist, but starting with a different SEXP type.
    buffer.clear();
    inject_header(SEXPType::LANG, input.attributes, buffer);
    writer.write(buffer.data(), buffer.size());

    // Attributes before the rest of the content.
    write_attributes(input.attributes, writer, buffer, shared);

    shared.write_symbol(input.function_name, input.function_encoding, writer, buffer);

    const auto& values = input.argument_values;
    const auto& has_tag = input.argument_has_name;
    const auto& tag_names = input.argument_names;
    const auto& tag_encodings = input.argument_encodings;
    size_t n = values.size();

    for (size_t i = 0; i < n; ++i) {
        buffer.clear();
        inject_next_pairlist_header(has_tag[i], buffer);
        writer.write(buffer.data(), buffer.size());

        if (has_tag[i]) {
            shared.write_symbol(tag_names[i], tag_encodings[i], writer, buffer);
        }
        write_object(values[i].get(), writer, buffer, shared); 
    }

    buffer.clear();
    inject_header(SEXPType::NILVALUE_, buffer);
    writer.write(buffer.data(), buffer.size());
    return;
}

}

#endif
