cmake_minimum_required(VERSION 3.29)
project(std_module_example CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# VCPKG
find_package(fmt CONFIG REQUIRED)


# Create Library
add_library(foo)
target_sources(foo
  PUBLIC
    FILE_SET CXX_MODULES FILES
      foo.cxx
)
target_link_libraries(foo
  PUBLIC
    fmt::fmt
)


# Create Executable
add_executable(hello main.cxx)
target_link_libraries(hello foo)