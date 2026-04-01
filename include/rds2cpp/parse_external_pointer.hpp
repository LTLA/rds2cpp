#ifndef RDS2CPP_PARSE_EXTERNAL_POINTER_HPP
#define RDS2CPP_PARSE_EXTERNAL_POINTER_HPP

#include <stdexcept>
#include <memory>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "parse_attributes.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_& src, SharedParseInfo& shared);

template<class Source_>
std::unique_ptr<ExternalPointerIndex> parse_external_pointer_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    const auto idx = request_new_external_pointer(shared);

    // Don't create a lvalue reference for shared.external_pointers[idx],
    // as this may be invalidated upon reallocation of 'shared.external_pointers'.
    shared.external_pointers[idx].protection = parse_object(src, shared);
    shared.external_pointers[idx].tag = parse_object(src, shared);
    if (has_attributes(header)) {
        parse_attributes(src, shared.external_pointers[idx].attributes, shared);
    }

    return std::make_unique<ExternalPointerIndex>(idx);
} catch (std::exception& e) {
    throw traceback("failed to parse an external pointer body", e);
    return std::unique_ptr<ExternalPointerIndex>();
}

}

#endif
