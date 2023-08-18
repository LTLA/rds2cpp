cmake_minimum_required(VERSION 3.24)

project(rds2cpp
    VERSION 1.0.0
    DESCRIPTION "Standalone C++ library for reading RDS files"
    LANGUAGES CXX)

add_library(rds2cpp INTERFACE)
add_library(ltla::rds2cpp ALIAS rds2cpp)

include(GNUInstallDirs)
target_include_directories(rds2cpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ltla_rds2cpp>)

target_compile_features(rds2cpp INTERFACE cxx_std_17)

option(RDS2CPP_FETCH_EXTERN "Automatically fetch rds2cpp's external dependencies." ON)
if(RDS2CPP_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(ltla_byteme CONFIG REQUIRED)
endif()

target_link_libraries(rds2cpp INTERFACE ltla::byteme)

# Install
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ltla_rds2cpp)

install(TARGETS rds2cpp
    EXPORT rds2cppTargets)

install(EXPORT rds2cppTargets
    FILE ltla_rds2cppTargets.cmake
    NAMESPACE ltla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_rds2cpp)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_rds2cppConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_rds2cpp)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ltla_rds2cppConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_rds2cpp)
