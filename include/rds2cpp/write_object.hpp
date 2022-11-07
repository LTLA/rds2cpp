#ifndef RDS2CPP_WRITE_OBJECT_HPP
#define RDS2CPP_WRITE_OBJECT_HPP

#include "RObject.hpp"
#include "SEXPType.hpp"
#include "write_atomic.hpp"
#include "write_list.hpp"
#include "write_s4.hpp"
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
        case SEXPType::VEC:
            write_list(object, writer, buffer);
            break;
        case SEXPType::S4:
            write_s4(object, writer, buffer);
            break;
        case SEXPType::NIL:
        case SEXPType::NILVALUE_:
            buffer.clear();
            buffer.resize(4);
            buffer[3] = static_cast<unsigned char>(SEXPType::NILVALUE_); // just using 255 consistently, as this seems to be what R uses for NULLs.
            writer.write(buffer.data(), buffer.size());
            break;
         default:
            throw std::runtime_error("unsupported SEXP type '" + std::to_string(static_cast<int>(object->type())) + "' for writing");
    }
    return;
}

}

#endif
