#ifndef RDS2CPP_WRITE_OBJECT_HPP
#define RDS2CPP_WRITE_OBJECT_HPP

#include "RObject.hpp"
#include "SEXPType.hpp"
#include "SharedWriteInfo.hpp"
#include "write_atomic.hpp"
#include "write_list.hpp"
#include "write_s4.hpp"
#include "write_pairlist.hpp"
#include "write_builtin.hpp"
#include "write_language.hpp"
#include "write_expression.hpp"
#include <vector>

namespace rds2cpp {

template<class BufferedWriter_>
void write_object(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    switch (object->type()) {
        case SEXPType::INT:
            write_integer(object, bufwriter, shared);
            break;
        case SEXPType::LGL:
            write_logical(object, bufwriter, shared);
            break;
        case SEXPType::REAL:
            write_double(object, bufwriter, shared);
            break;
        case SEXPType::RAW:
            write_raw(object, bufwriter, shared);
            break;
        case SEXPType::CPLX:
            write_complex(object, bufwriter, shared);
            break;
        case SEXPType::STR:
            write_string(object, bufwriter, shared);
            break;
        case SEXPType::VEC:
            write_list(object, bufwriter, shared);
            break;
        case SEXPType::S4:
            write_s4(object, bufwriter, shared);
            break;
        case SEXPType::NIL:
        case SEXPType::NILVALUE_:
            inject_header(SEXPType::NILVALUE_, bufwriter);
            break;
        case SEXPType::LIST:
            write_pairlist(object, bufwriter, shared);
            break;
        case SEXPType::SYM:
            write_symbol(object, bufwriter, shared);
            break;
        case SEXPType::BUILTIN:
            write_builtin(object, bufwriter, shared);
            break;
        case SEXPType::LANG:
            write_language(object, bufwriter, shared);
            break;
        case SEXPType::EXPR:
            write_expression(object, bufwriter, shared);
            break;
        case SEXPType::ENV: 
        case SEXPType::GLOBALENV_:
        case SEXPType::BASEENV_:
        case SEXPType::EMPTYENV_:
            write_environment(object, bufwriter, shared);
            break;
        case SEXPType::EXTPTR:
            write_external_pointer(object, bufwriter, shared);
            break;
        default:
            throw std::runtime_error("unsupported SEXP type '" + std::to_string(static_cast<int>(object->type())) + "' for writing");
    }
    return;
}

}

#endif
