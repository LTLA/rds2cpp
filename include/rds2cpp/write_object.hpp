#ifndef RDS2CPP_WRITE_OBJECT_HPP
#define RDS2CPP_WRITE_OBJECT_HPP

#include "RObject.hpp"
#include "SEXPType.hpp"
#include "SharedWriteInfo.hpp"
#include "write_atomic.hpp"
#include "write_list.hpp"
#include "write_s4.hpp"
#include "write_pairlist.hpp"
#include <vector>

namespace rds2cpp {

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer, SharedWriteInfo& shared) {
    switch (object->type()) {
        case SEXPType::INT:
            write_integer(object, writer, buffer, shared);
            break;
        case SEXPType::LGL:
            write_logical(object, writer, buffer, shared);
            break;
        case SEXPType::REAL:
            write_double(object, writer, buffer, shared);
            break;
        case SEXPType::RAW:
            write_raw(object, writer, buffer, shared);
            break;
        case SEXPType::CPLX:
            write_complex(object, writer, buffer, shared);
            break;
        case SEXPType::STR:
            write_string(object, writer, buffer, shared);
            break;
        case SEXPType::VEC:
            write_list(object, writer, buffer, shared);
            break;
        case SEXPType::S4:
            write_s4(object, writer, buffer, shared);
            break;
        case SEXPType::NIL:
        case SEXPType::NILVALUE_:
            buffer.clear();
            buffer.resize(4);
            buffer[3] = static_cast<unsigned char>(SEXPType::NILVALUE_); // just using 255 consistently, as this seems to be what R uses for NULLs.
            writer.write(buffer.data(), buffer.size());
            break;
        case SEXPType::LIST:
            write_pairlist(object, writer, buffer, shared);
            break;
        default:
            throw std::runtime_error("unsupported SEXP type '" + std::to_string(static_cast<int>(object->type())) + "' for writing");
    }
    return;
}

}

#endif