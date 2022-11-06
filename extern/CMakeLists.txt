cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG 52bf754cea80cc079092df2eef96a534876d0300
)

FetchContent_MakeAvailable(byteme)
