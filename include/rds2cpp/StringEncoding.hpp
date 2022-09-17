#ifndef RDS2CPP_STRING_ENCODING_HPP
#define RDS2CPP_STRING_ENCODING_HPP

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

}

#endif
