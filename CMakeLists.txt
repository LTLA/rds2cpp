cmake_minimum_required(VERSION 3.14)

project(rds2cpp
    VERSION 1.0.0
    DESCRIPTION "C++ library for reading RDS files"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(rds2cpp INTERFACE)

target_include_directories(rds2cpp INTERFACE include/)

add_subdirectory(extern)

target_link_libraries(rds2cpp INTERFACE byteme)
