#ifndef RDS2CPP_PARSE_EXTERNAL_POINTER_HPP
#define RDS2CPP_PARSE_EXTERNAL_POINTER_HPP

#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>

#include "RObject.hpp"
#include "SharedParseInfo.hpp"
#include "parse_attributes.hpp"

namespace rds2cpp {

template<class Source_>
std::unique_ptr<RObject> parse_object(Source_& src, SharedParseInfo& shared);

template<class Source_>
ExternalPointerIndex parse_external_pointer_body(Source_& src, const Header& header, SharedParseInfo& shared) try {
    auto idx = shared.request_external_pointer();
    auto& extptr = shared.external_pointers[idx];

    extptr.protection = parse_object(src, shared);
    extptr.tag = parse_object(src, shared);
    if (has_attributes(header)) {
        parse_attributes(src, extptr.attributes, shared);
    }

    return ExternalPointerIndex(idx);
} catch (std::exception& e) {
    throw traceback("failed to parse an external pointer body", e);
}

}

#endif
