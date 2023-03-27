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

template<class Reader>
std::unique_ptr<RObject> parse_object(Reader& reader, std::vector<unsigned char>& leftovers, SharedParseInfo& shared);

template<class Reader>
ExternalPointerIndex parse_external_pointer_body(Reader& reader, std::vector<unsigned char>& leftovers, const Header& header, SharedParseInfo& shared) try {
    auto idx = shared.request_external_pointer();
    auto& extptr = shared.external_pointers[idx];

    extptr.protection = parse_object(reader, leftovers, shared);
    extptr.tag = parse_object(reader, leftovers, shared);
    if (has_attributes(header)) {
        parse_attributes(reader, leftovers, extptr.attributes, shared);
    }

    return ExternalPointerIndex(idx);
} catch (std::exception& e) {
    throw traceback("failed to parse an external pointer body", e);
}

}

#endif
