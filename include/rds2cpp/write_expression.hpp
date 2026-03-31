#ifndef RDS2CPP_WRITE_EXPRESSION_HPP
#define RDS2CPP_WRITE_EXPRESSION_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "SharedWriteInfo.hpp"
#include "utils_write.hpp"
#include "write_attributes.hpp"

namespace rds2cpp {

template<class BufferedWriter_>
void write_object(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared);

template<class BufferedWriter_>
void write_expression(const RObject* object, BufferedWriter_& bufwriter, SharedWriteInfo& shared) {
    auto ptr = static_cast<const ExpressionVector*>(object);
    inject_header(*ptr, bufwriter, shared);

    const auto len = sanisizer::cast<std::size_t>(ptr->data.size());
    inject_length(len, bufwriter);

    for (const auto& x : ptr->data) {
        write_object(x.get(), bufwriter, shared);
    }
    write_attributes(ptr->attributes, bufwriter, shared);
}

}

#endif
