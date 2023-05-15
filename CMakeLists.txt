cmake_minimum_required(VERSION 3.8)

project(hlc
  VERSION 0.1.0
  LANGUAGES C CXX
)

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(bench)
