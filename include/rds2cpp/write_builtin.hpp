#ifndef RDS2CPP_WRITE_BUILTIN_HPP
#define RDS2CPP_WRITE_BUILTIN_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"
#include "write_attributes.hpp"

namespace rds2cpp {

template<class Writer>
void write_builtin(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer) {
    auto ptr = static_cast<const BuiltInFunction*>(object);

    buffer.clear();
    inject_header(*ptr, buffer);

    size_t len = ptr->name.size();
    inject_length(len, buffer);

    inject_string(ptr->name.c_str(), len, buffer);
    writer.write(buffer.data(), buffer.size());
    return;
}

}

#endif
