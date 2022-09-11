#ifndef RDS2CPP_STRING_ENCODING_HPP
#define RDS2CPP_STRING_ENCODING_HPP

namespace rds2cpp {

enum class StringEncoding : unsigned char { 
    NONE, 
    LATIN1, 
    UTF8, 
    ASCII 
};

}

#endif
