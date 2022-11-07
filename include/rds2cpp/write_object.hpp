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
            write_integer(object, writer, buffer);
            break;
        case SEXPType::LGL:
            write_logical(object, writer, buffer);
            break;
        case SEXPType::REAL:
            write_double(object, writer, buffer);
            break;
        case SEXPType::RAW:
            write_raw(object, writer, buffer);
            break;
        case SEXPType::CPLX:
            write_complex(object, writer, buffer);
            break;
        case SEXPType::STR:
            write_string(object, writer, buffer);
            break;
         default:
            throw std::runtime_error("unsupported SEXP type '" + std::to_string(static_cast<int>(object->type())) + "' for writing");
    }
    return;
}

}

#endif
