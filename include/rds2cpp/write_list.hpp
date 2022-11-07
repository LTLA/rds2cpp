#ifndef RDS2CPP_WRITE_LIST_HPP
#define RDS2CPP_WRITE_LIST_HPP

#include <memory>
#include <vector>

#include "RObject.hpp"
#include "utils.hpp"
#include "write_attributes.hpp"

namespace rds2cpp {

template<class Writer>
void write_object(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer);

template<class Writer>
void write_list(const RObject* object, Writer& writer, std::vector<unsigned char>& buffer) {
    auto ptr = reinterpret_cast<const GenericVector*>(object);
    buffer.clear();
    inject_header<true>(*ptr, buffer);

    size_t len = ptr->data.size();
    inject_length(len, buffer);
    writer.write(buffer.data(), buffer.size());

    for (size_t i = 0; i < len; ++i) {
        write_object(ptr->data[i].get(), writer, buffer);
    }
    write_attributes(ptr->attributes, writer, buffer);
    return;
}

}

#endif
