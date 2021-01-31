cmake_minimum_required(VERSION 3.10)

project(dbc)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add in the debug flag0.
set(GCC_COVERAGE_COMPILE_FLAGS "-g")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# add where to find the source files
list(APPEND SOURCE ${PROJECT_SOURCE_DIR}/src/util/utils.cpp)

include_directories(src)
include_directories(include)

add_subdirectory(test)
add_subdirectory(doc)

add_library(${PROJECT_NAME} STATIC ${SOURCE})
