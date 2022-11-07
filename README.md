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

// Returns an object containing the file information,
// e.g., R version used to read/write the file.
auto file_info = rds2cpp::parse_rds(fpath);

// Get the pointer to the actual R object.
const auto& ptr = file_info->object;
```

The type of the underlying object can then be queried for further examination.
For example, if we wanted to process integer vectors:

```cpp
if (ptr->type() == rds2cpp::SEXPType::INT) {
    auto iptr = static_cast<const rds2cpp::IntegerVector*>(ptr.get());
    const auto& values = iptr->data; // vector of int32_t's.
    const auto& attr_names = iptr->attributes.names; // vector of attribute names.
}
```

Or maybe we have an ordinary list:

```cpp
if (ptr->type() == rds2cpp::SEXPType::VEC) {
    auto lptr = static_cast<const rds2cpp::GenericVector*>(ptr.get());
    const auto& elements = lptr->data; // vector of pointers to list elements.
}
```

Slots of S4 instances are encoded in the attributes:

```cpp
if (ptr->type() == rds2cpp::SEXPType::S4) {
    auto sptr = static_cast<const rds2cpp::S4Object*>(ptr.get());
    const auto& slot_names = sptr->attributes.names;
    const auto& slot_values = sptr->attributes.values;
}
```

See the [reference documentation](https://ltla.github.io/rds2cpp) for a list of known representations.

## Writing RDS files

The `write_rds()` function will write RDS files from an `rds2cpp::RObject` representation:

```cpp
rds2cpp::RdsFile file_info;

// Setting up an integer vector.
auto vec = new rds2cpp::IntegerVector;
file_info.object.reset(vec);

// Storing data in the integer vector.
vec->data = std::vector<int32_t>{ 0, 1, 2, 3, 4, 5 };

rds2cpp::write_rds(file_info, "some_file_path.rds");
```

Here's a more complicated example that saves a sparse matrix (as a `dgCMatrix` from the **Matrix** package) to file.

```cpp
rds2cpp::RdsFile file_info;
auto ptr = new rds2cpp::S4Object;
file_info.object.reset(ptr);
auto& obj = *ptr;

obj.class_name = "dgCMatrix";
obj.class_encoding = rds2cpp::StringEncoding::UTF8;
obj.package_name = "Matrix";
obj.package_encoding = rds2cpp::StringEncoding::UTF8;

obj.attributes.names.push_back("i");
obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
auto ivec = new rds2cpp::IntegerVector;
obj.attributes.values.emplace_back(ivec);
ivec->data = std::vector<int32_t>{ 6, 8, 0, 3, 5, 6, 0, 1, 3, 7 };

obj.attributes.names.push_back("p");
obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
auto pvec = new rds2cpp::IntegerVector;
obj.attributes.values.emplace_back(pvec);
pvec->data = std::vector<int32_t>{ 0, 0, 2, 3, 4, 5, 6, 8, 8, 8, 10 };

obj.attributes.names.push_back("x");
obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
auto xvec = new rds2cpp::DoubleVector;
obj.attributes.values.emplace_back(xvec);
xvec->data = std::vector<double>{ 0.96, -0.34, 0.82, -2, -0.072, 0.39, 0.16, 0.36, -1.5, -0.047 };

obj.attributes.names.push_back("Dim");
obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
auto dims = new rds2cpp::IntegerVector;
obj.attributes.values.emplace_back(dims);
dims->data = std::vector<int32_t>{ 10, 10 };

obj.attributes.names.push_back("Dimnames");
obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
auto dimnames = new rds2cpp::GenericVector;
obj.attributes.values.emplace_back(dimnames);
dimnames->data.emplace_back(new rds2cpp::Null);
dimnames->data.emplace_back(new rds2cpp::Null);

obj.attributes.names.push_back("factors");
obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
auto factors = new rds2cpp::GenericVector;
obj.attributes.values.emplace_back(factors);

rds2cpp::write_rds(file_info, "my_matrix.rds");
``` 

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
