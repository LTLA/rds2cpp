#ifndef RDS2CPP_WRITE_BUILTIN_HPP
#define RDS2CPP_WRITE_BUILTIN_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"
#include "write_attributes.hpp"

namespace rds2cpp {

template<class BufferedWriter_>
void write_builtin(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    auto ptr = static_cast<const BuiltInFunction*>(object);
    inject_header(*ptr, bufwriter, shared);

    const auto len = sanisizer::cast<std::size_t>(ptr->name.size());
    inject_length(len, bufwriter);
    bufwriter.write(ptr->name);
}

}

#endif
