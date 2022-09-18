# Read RDS files in C++

![Unit tests](https://github.com/LTLA/rds2cpp/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/rds2cpp/actions/workflows/doxygenate.yaml/badge.svg)

## Overview

This repository contains a header-only C++ library for reading RDS files into memory.
This provides a lightweight mechanism for applications to read user data created with `saveRDS()` without linking to R itself.
We currently support most user-visible data structures such as atomic vectors, lists, environments and S4 classes.

## Quick start

Given a path to an RDS file, the `parse_rds()` function will return a pointer to an `RObject` interface:

```cpp
#include "rds2cpp/rds2cpp.hpp"

auto ptr = rds2cpp::parse_rds(fpath);
```

The type of the underlying object can then be queried for further examination.
For example, if we wanted to process integer vectors:

```cpp
if (ptr->type() == rds2cpp::SEXPType::INT) {
    auto iptr = static_cast<rds2cpp::IntegerVector*>(ptr.get());
    const auto& values = iptr->data; // vector of int32_t's.
    const auto& attr_names = iptr->attributes.names; // vector of attribute names.
}
```

Or maybe we have a list instead:

```cpp
if (ptr->type() == rds2cpp::SEXPType::VEC) {
    auto lptr = static_cast<rds2cpp::List*>(ptr.get());
    const auto& elements = lptr->data; // vector of pointers to list elements.
}
```

Slots of S4 instances are encoded in the attributes:

```cpp
if (ptr->type() == rds2cpp::SEXPType::S4) {
    auto sptr = static_cast<rds2cpp::S4Object*>(ptr.get());
    const auto& slot_names = sptr->attributes.names;
    const auto& slot_values = sptr->attributes.values;
}
```

See the [reference documentation](https://ltla.github.io/rds2cpp) for a list of known representations.

## Building projects

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```
include(FetchContent)

FetchContent_Declare(
  rds2cpp
  GIT_REPOSITORY https://github.com/LTLA/rds2cpp
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(rds2cpp)
```

Then you can link to **rds2cpp** to make the headers available during compilation:

```
# For executables:
target_link_libraries(myexe rds2cpp)

# For libaries
target_link_libraries(mylib INTERFACE rds2cpp)
```

## Known limitations

This library may not support RDS files created using `saveRDS()` with non-default parameters.

Currently, no support is provided for unserializing built-in functions or user-defined closures.
