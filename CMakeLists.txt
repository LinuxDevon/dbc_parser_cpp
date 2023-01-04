cmake_minimum_required(VERSION 3.10)

project(dbc VERSION 0.1.1 DESCRIPTION "C++ DBC Parser")

# package
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
include(CPack)

option(DEBUG "use debug flag" NO)

# defines variables used in the dbc.pc.in
include(GNUInstallDirs)

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
# file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/ *.cpp)
list(APPEND SOURCE ${PROJECT_SOURCE_DIR}/src/utils.cpp
				   ${PROJECT_SOURCE_DIR}/src/message.cpp
				   ${PROJECT_SOURCE_DIR}/src/signal.cpp
				   ${PROJECT_SOURCE_DIR}/src/dbc.cpp)

include_directories(src)
include_directories(include)

add_subdirectory(test)
add_subdirectory(doc)

add_library(${PROJECT_NAME} STATIC ${SOURCE})

add_custom_target(release
					WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					COMMAND ${CMAKE_SOURCE_DIR}/scripts/create_single_header.sh
					DEPENDS ${PROJECT_NAME})

# Generate pkg-config file
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
