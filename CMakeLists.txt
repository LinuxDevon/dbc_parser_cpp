cmake_minimum_required(VERSION 3.10)

project(dbc)

option(DEBUG "use debug flag" NO)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_COMPILE_FLAGS "-Wextra -Wall -Wfloat-equal -Wundef -Wshadow \
-Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion \
-Wunreachable-code -Wformat=2 -Werror -Wuninitialized -Winit-self")

if(DEBUG)
	set(GCC_COMPILE_FLAGS ${GCC_COMPILE_FLAGS}" -g")
else()
	set(GCC_COMPILE_FLAGS ${GCC_COMPILE_FLAGS}" -O2")
endif()

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# add where to find the source files
list(APPEND SOURCE ${PROJECT_SOURCE_DIR}/src/utils.cpp
				   ${PROJECT_SOURCE_DIR}/src/dbc.cpp)

include_directories(src)
include_directories(include)

add_subdirectory(test)
add_subdirectory(doc)

add_library(${PROJECT_NAME} STATIC ${SOURCE})
