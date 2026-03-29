cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG master # ^2.1.3
)

FetchContent_Declare(
  sanisizer 
  GIT_REPOSITORY https://github.com/LTLA/sanisizer
  GIT_TAG master # ^0.2.2
)

FetchContent_MakeAvailable(byteme)
FetchContent_MakeAvailable(sanisizer)
