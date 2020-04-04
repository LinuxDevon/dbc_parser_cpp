cmake_minimum_required(VERSION 3.10)

project(DBC_Tests)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add in the debug flag
set(GCC_COVERAGE_COMPILE_FLAGS "-g")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# add where to find the source files
include_directories(src)
include_directories(test)
add_subdirectory(test)
add_subdirectory(doc)

