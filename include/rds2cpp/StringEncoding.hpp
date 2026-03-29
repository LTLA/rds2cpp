#ifndef RDS2CPP_STRING_ENCODING_HPP
#define RDS2CPP_STRING_ENCODING_HPP

#include <string>

/**
 * @file StringEncoding.hpp
 *
 * @brief String encodings.
 */

namespace rds2cpp {

/**
 * Encoding options for R strings.
 */
enum class StringEncoding : unsigned char { 
    NONE, 
    LATIN1, 
    UTF8, 
    ASCII 
};

/**
 * @cond
 */
// The name is based on the names in ?Encoding... kinda hard to see if this is valid.
inline StringEncoding string_encoding_from_name(const std::string& name) {
    if (name == "UTF-8") {
        return StringEncoding::UTF8;
    } else if (name == "latin1") {
        return StringEncoding::LATIN1;
    } else if (name == "bytes") {
        return StringEncoding::NONE;
    } else { // be more permissive if it's none of the above, rather than forcing it to be 'unknown'.
        return StringEncoding::ASCII;
    }
}

inline std::string string_encoding_to_name(StringEncoding encoding) {
    std::string name;
    switch (encoding) {
        case rds2cpp::StringEncoding::NONE: name = "bytes"; break;
        case rds2cpp::StringEncoding::UTF8: name = "UTF-8"; break;
        case rds2cpp::StringEncoding::ASCII: name = "unknown"; break;
        case rds2cpp::StringEncoding::LATIN1: name = "latin1"; break;
    }
    return name;
}
/**
 * @endcond
 */

}

#endif
