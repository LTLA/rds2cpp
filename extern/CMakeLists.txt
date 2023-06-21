cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG c0e0cf99cbb8cd099556c03adcabc06ae0ec3337
)

FetchContent_MakeAvailable(byteme)
