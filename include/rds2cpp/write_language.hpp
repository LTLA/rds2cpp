#ifndef RDS2CPP_WRITE_LANGUAGE_HPP
#define RDS2CPP_WRITE_LANGUAGE_HPP

#include <vector>
#include <memory>
#include <stdexcept>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"

namespace rds2cpp {

template<class BufferedWriter_>
void write_object(const RObject*, BufferedWriter_&, SharedWriteInfo&);

template<class BufferedWriter_>
void write_language(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    const LanguageObject& input = *static_cast<const LanguageObject*>(object);

    // Just another pairlist, but starting with a different SEXP type.
    inject_header(SEXPType::LANG, input.attributes, bufwriter);

    // Attributes before the rest of the content.
    write_attributes(input.attributes, bufwriter, shared);

    shared.write_symbol(input.function_name, input.function_encoding, bufwriter);

    const auto& values = input.argument_values;
    const auto& has_tag = input.argument_has_name;
    const auto& tag_names = input.argument_names;
    const auto& tag_encodings = input.argument_encodings;
    const auto n = values.size();

    for (I<decltype(n)> i = 0; i < n; ++i) {
        inject_next_pairlist_header(has_tag[i], bufwriter);
        if (has_tag[i]) {
            shared.write_symbol(tag_names[i], tag_encodings[i], bufwriter);
        }
        write_object(values[i].get(), bufwriter, shared); 
    }

    inject_header(SEXPType::NILVALUE_, bufwriter);
}

}

#endif
