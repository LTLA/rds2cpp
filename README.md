# Read RDS/RDA files in C++

![Unit tests](https://github.com/LTLA/rds2cpp/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/rds2cpp/actions/workflows/doxygenate.yaml/badge.svg)

## Overview

This repository contains a header-only C++ library for reading and writing RDS or RDA files without the need to link to R's libraries.
In this manner, we can use RDS as a flexible data exchange format across different frameworks that have C++ bindings, 
e.g., [Python](https://github.com/biocpy/rds2py), [Javascript (via Wasm)](https://github.com/jkanche/scran.js).
We currently support most user-visible data structures such as atomic vectors, lists, environments and S4 classes.

## Quick start

Each RDS file contains a single R object and is typically created by calling `saveRDS()` within an R session.
Given a path to an RDS file, the `parse_rds()` function will return a pointer to an `RObject` interface:

```cpp
#include "rds2cpp/rds2cpp.hpp"

// Returns an object containing the file information,
// e.g., R version used to read/write the file.
auto file_info = rds2cpp::parse_rds(fpath, rds2cpp::ParseRdsOptions());

// Get the pointer to the actual R object.
const auto& ptr = file_info->object;
```

The type of the underlying object can then be queried for further examination.
For example, if we wanted to process integer vectors:

```cpp
if (ptr->type() == rds2cpp::SEXPType::INT) {
    auto iptr = static_cast<const rds2cpp::IntegerVector*>(ptr.get());
    const std::vector<std::int32_t>& values = iptr->data;
}
```

See the [reference documentation](https://ltla.github.io/rds2cpp) for a list of known representations.

## More reading examples

We can extract ordinary lists from an RDS file, examining the attributes to determine if the list is named.

```cpp
if (ptr->type() == rds2cpp::SEXPType::VEC) {
    auto lptr = static_cast<const rds2cpp::GenericVector*>(ptr.get());
    const auto& elements = lptr->data; // vector of pointers to list elements.

    for (const auto& attr : lptr->attributes) {
        // Symbols are referenced by their position in the 'symbols' vector.
        const auto& attr_name = file_info.symbols[attr.name.index].name;

        if (attr_name == "names") {
            if (attr.value->type() != rds2cpp::SEXPType::STR) {
                // Just adding some protection for weird objects.
                throw std::runtime_error("oops, names should be strings!");
            }

            auto nptr = static_cast<const rds2cpp::StringVector*>(attr.value.get());
            for (const auto& str : nptr->value) {
                if (!str.value.has_value()) {
                    throw std::runtime_error("oops, names should not be missing!");
                }

                const std::string& str_value = *(str.value); // value of the string.
                const auto& str_enc = str.encoding; // encoding of the string.
                // Do something with the list names...
            }
        }
    }
}
```

Slots of S4 instances are similarly encoded in the attributes -
except for the class name, which is extracted into its own member.

```cpp
if (ptr->type() == rds2cpp::SEXPType::S4) {
    auto sptr = static_cast<const rds2cpp::S4Object*>(ptr.get());
    sptr->class_name;
    sptr->package_name;

    for (const auto& slot : sptr->attributes) {
        const auto& slot_name = file_info.symbols[slot.name.index].name;
        const auto& slot_val = *(slot.value); // Do something with the slot value...
    }
}
```

Advanced users can also pull out serialized environments.
These should be treated as file-specific globals that may be referenced one or more times inside the R object.

```cpp
if (ptr->type() == rds2cpp::SEXPType::ENV) {
    auto eptr = static_cast<const rds2cpp::EnvironmentIndex*>(ptr.get());
    const auto& env = file_info.environments[eptr->index];

    for (const auto& var = env.variables) {
        const auto& var_name = file_info.symbols[var.name.index].name;
        const auto& var_value = *(var.value); // Do something with the variable...
    }
}
```

`NULL`s are supported but not particularly interesting:

```cpp
if (ptr->type() == rds2cpp::SEXPType::NIL) {
   // Do something.
}
```

## Writing RDS files

The `write_rds()` function will write RDS files from an `rds2cpp::RObject` representation:

```cpp
rds2cpp::RdsFile file_info;

// Setting up an integer vector.
auto vec = new rds2cpp::IntegerVector;
file_info.object.reset(vec);

// Storing data in the integer vector.
vec->data = std::vector<std::int32_t>{ 0, 1, 2, 3, 4, 5 };

rds2cpp::write_rds(file_info, "some_file_path.rds", rds2cpp::WriteRdsOptions());
```

Here's a more complicated example that saves a sparse matrix (as a `dgCMatrix` from the **Matrix** package) to file.

```cpp
rds2cpp::RdsFile file_info;
auto ptr = std::make_unique<rds2cpp::S4Object>();

auto& obj = *ptr;
obj.class_name = "dgCMatrix";
obj.package_name = "Matrix";

auto ivec = std::make_unique<rds2cpp::IntegerVector>();
ivec->data = std::vector<std::int32_t>{ 6, 8, 0, 3, 5, 6, 0, 1, 3, 7 };
obj.attributes.emplace_back(
    rds2cpp::register_symbol("i", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(ivec)
);

auto pvec = std::make_unique<rds2cpp::IntegerVector>();
pvec->data = std::vector<std::int32_t>{ 0, 0, 2, 3, 4, 5, 6, 8, 8, 8, 10 };
obj.attributes.emplace_back(
    rds2cpp::register_symbol("p", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(pvec)
);

auto xvec = std::make_unique<rds2cpp::DoubleVector>();
xvec->data = std::vector<double>{ .96, -.34, .82, -2., -.72, .39, .16, .36, -1.5, -.47 };
obj.attributes.emplace_back(
    rds2cpp::register_symbol("x", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(xvec)
);

auto dims = std::make_unique<rds2cpp::IntegerVector>();
dims->data = std::vector<int32_t>{ 10, 10 };
obj.attributes.emplace_back(
    rds2cpp::register_symbol("Dim", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(dims)
);

auto dimnames = std::make_unique<rds2cpp::GenericVector>();
dimnames->data.emplace_back(new Null);
dimnames->data.emplace_back(new Null);
obj.attributes.emplace_back(
    rds2cpp::register_symbol("Dimnames", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(dimnames)
);

obj.attributes.add(
    rds2cpp::register_symbol("factors", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::make_unique<rds2cpp::GenericVector>()
);

file_info.object = std::move(ptr);
rds2cpp::write_rds(file_info, "my_matrix.rds", {});
``` 

We can also create environments by registering the environment before creating indices to it.

```cpp
rds2cpp::RdsFile file_info;

// Creating an environment with a 'foo' variable containing c('bar', NA, 'whee')
file_info.environments.emplace_back();
auto& current_env = file_info.environments.back();

auto sptr = std::make_unique<rds2cpp::StringVector>();
sptr->data.emplace_back("bar", rds2cpp::StringEncoding::UTF8);
sptr->data.emplace_back(); // NA string.
sptr->data.emplace_back("whee", rds2cpp::StringEncoding::ASCII);

// The object is just a reference to the first environment: 
file_info.object.reset(new rds2cpp::EnvironmentIndex(0));

rds2cpp::write_rds(file_info, "my_env.rds", {});
```

## Reading/writing RDA files

Each RDA file (a.k.a., Rdata) contains multiple R objects, each of which is associated with a unique name.
It is typically created by calling `save()` within an R session.
We can read all objects into memory with the `parse_rda()` function:

```cpp
auto file_info = rds2cpp::parse_rda(fpath, rds2cpp::ParseRdaOptions());

for (const auto& obj : file_info.objects) {
    const auto& obj_name = file_info.symbols[obj.name.index].name;
    switch (obj.value->type()) {
        case rds2cpp::SEXPType::INT:
            // This is an integer vector...
            break;
        case rds2cpp::SEXPType::STR:
            // This is a character vector...
            break;
        default:
            // and so on...
    }
}
```

Similarly, we can write the name/object pairs into an RDA file.

```cpp
#include <numeric>

auto ivec = std::make_unique<rds2cpp::IntegerVector>(5);
std::iota(ivec->data.begin(), ivec->data.end(), 1);

auto list = std::make_unique<rds2cpp::GenericVector>(2);
list->data[0].reset(new Null);
list->data[1].reset(new rds2cpp::LogicalVector(10));

auto svec = std::make_unique<rds2cpp::StringVector>(1);
svec->data[0].value = "FOOBAR";

rds2cpp::RdaFile file_info;
file_info.objects.emplace_back(
    rds2cpp::register_symbol("alpha", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(ivec)
);
file_info.objects.emplace_back(
    rds2cpp::register_symbol("bravo", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(list)
);
file_info.objects.emplace_back(
    rds2cpp::register_symbol("charlie", rds2cpp::StringEncoding::UTF8, file_info.symbols),
    std::move(svec)
);

rds2cpp::write_rda(file_info, "my_env.Rda", rds2cpp::WriteRdaOptions());
```

## Building projects

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  rds2cpp
  GIT_REPOSITORY https://github.com/LTLA/rds2cpp
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(rds2cpp)
```

Then you can link to **rds2cpp** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe rds2cpp)

# For libaries
target_link_libraries(mylib INTERFACE rds2cpp)
```

### CMake using `find_package()`

You can install the library by cloning a suitable version of this repository and running the following commands:

```sh
mkdir build && cd build
cmake .. 
cmake --build . --target install
```

Then you can use `find_package()` as usual:

```cmake
find_package(ltla_rds2cpp CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE ltla::rds2cpp)
```

### Manual

If you're not using CMake, the simple approach is to just copy the files in the [`include/`](include) subdirectory -
either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
You'll need to add the various dependencies listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt) to the compiler's search path.
You'll also need to link to Zlib.

## Known limitations

This library may not support RDS files created using `saveRDS()` with non-default parameters.

Environments are written without a hash table, so as to avoid the need to replicate R's string hashing logic.
This may result in slower retrieval of variables when those environments are loaded into an R session.

Currently, no support is provided for unserializing built-in functions or user-defined closures.
