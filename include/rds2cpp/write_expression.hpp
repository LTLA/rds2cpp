#ifndef RDS2CPP_WRITE_EXPRESSION_HPP
#define RDS2CPP_WRITE_EXPRESSION_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"
#include "write_attributes.hpp"

namespace rds2cpp {

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared);

template<class Writer>
void write_expression(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    auto ptr = static_cast<const ExpressionVector*>(object);
    buffer.clear();
    inject_header(*ptr, buffer);

    size_t len = ptr->data.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    for (size_t i = 0; i < len; ++i) {
        write_object(ptr->data[i].get(), writer, buffer, shared);
    }
    write_attributes(ptr->attributes, writer, buffer, shared);
    return;
}

}

#endif
