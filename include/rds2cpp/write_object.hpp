#ifndef RDS2CPP_WRITE_OBJECT_HPP
#define RDS2CPP_WRITE_OBJECT_HPP

#include "RObject.hpp"
#include "SEXPType.hpp"
#include "write_atomic.hpp"
#include <vector>

namespace rds2cpp {

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer) {
    switch (object->type()) {
        case SEXPType::INT:
            write_integer(*reinterpret_cast<const IntegerVector*>(object), writer, buffer);
            break;
         default:
            throw std::runtime_error("unsupported SEXP type '" + std::to_string(static_cast<int>(object->type())) + "' for writing");
    }
    return;
}

}

#endif
