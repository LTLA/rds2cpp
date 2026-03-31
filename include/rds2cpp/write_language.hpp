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
    inject_header(SEXPType::LANG, input.attributes, bufwriter, shared);

    // Attributes before the rest of the content.
    write_attributes(input.attributes, bufwriter, shared);

    shared.write_symbol(&(input.function), bufwriter);

    for (const auto& arg : input.arguments) {
        inject_next_pairlist_header(arg.name.has_value(), bufwriter);
        if (arg.name.has_value()) {
            shared.write_symbol(&(*(arg.name)), bufwriter);
        }
        write_object(arg.value.get(), bufwriter, shared); 
    }

    inject_header(SEXPType::NILVALUE_, bufwriter);
}

}

#endif
