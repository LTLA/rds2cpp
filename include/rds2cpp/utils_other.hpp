#ifndef RDS2CPP_UTILS_OTHER_HPP
#define RDS2CPP_UTILS_OTHER_HPP

#include <type_traits>

namespace rds2cpp {

template<typename Type_>
using I = std::remove_cv_t<std::remove_reference_t<Type_> >;

}

#endif
