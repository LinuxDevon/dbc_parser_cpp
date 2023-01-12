cmake_minimum_required(VERSION 3.23)
# FILE_SET needs cmake 3.23

project(dbc VERSION 0.1.1 DESCRIPTION "C++ DBC Parser")

# package
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
include(CPack)

option(DEBUG "use debug flag" NO)
option(ENABLE_TESTS "Enable Unittests" ON)

# defines variables used in the dbc.pc.in
include(GNUInstallDirs)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(FastFloat REQUIRED)

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

add_subdirectory(third_party/bitstream)

# add where to find the source files
# file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/ *.cpp)
list(APPEND SOURCE ${PROJECT_SOURCE_DIR}/src/utils.cpp
				   ${PROJECT_SOURCE_DIR}/src/message.cpp
				   ${PROJECT_SOURCE_DIR}/src/signal.cpp
				   ${PROJECT_SOURCE_DIR}/src/dbc.cpp)

set(HEADER_FILES
  ${PROJECT_SOURCE_DIR}/include/libdbc/dbc.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/message.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/signal.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/utils/utils.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/exceptions/error.hpp
)

include_directories(src)
include_directories(include)

if(ENABLE_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

add_subdirectory(doc)

add_library(${PROJECT_NAME} STATIC ${SOURCE})
target_link_libraries(${PROJECT_NAME} bitstream)
target_sources(${PROJECT_NAME} INTERFACE FILE_SET HEADERS
	TYPE HEADERS
	BASE_DIRS ${PROJECT_SOURCE_DIR}/include/libdbc
	FILES ${HEADER_FILES})

add_custom_target(release
					WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					COMMAND ${CMAKE_SOURCE_DIR}/scripts/create_single_header.sh
					DEPENDS ${PROJECT_NAME})

## Installation
# install lib
install(TARGETS ${PROJECT_NAME}
		DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install headers
install(TARGETS ${PROJECT_NAME}
		FILE_SET HEADERS
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lib${PROJECT_NAME}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Generate pkg-config file
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
