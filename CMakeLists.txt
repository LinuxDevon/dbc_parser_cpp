cmake_minimum_required(VERSION 3.16)

project(dbc VERSION 0.2.0 DESCRIPTION "C++ DBC Parser")

# -- PROJECT OPTIONS  -- #
option(DEBUG "use debug flag" NO)
option(DBC_ENABLE_TESTS "Enable Unittests" ON)
# Turn OFF, if you are using FetchContent to include it to your project
option(DBC_FETCH_CONTENT_INCLUSION "Include project with FetchContent_Declare in another project. In this case the headers and the cmake files are not needed, only the library" OFF)
option(DBC_GENERATE_DOCS "Use doxygen if installed to generated documentation files" OFF)
# ---------------------- #

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# package
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
include(CPack)

# defines variables used in the dbc.pc.in
include(GNUInstallDirs)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(FastFloat QUIET)
if (NOT ${FastFloat_FOUND})
    include(FetchContent)
    FetchContent_Declare(
      FastFloat
      GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
      GIT_TAG 1ea4f27b2aeee2859a1354a3c24cff52a116cad1
    )
    # FetchContent_MakeAvailable(FastFloat)
    FetchContent_Populate(FastFloat)
    add_subdirectory(${fastfloat_SOURCE_DIR} ${fastfloat_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

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

set(HEADER_FILES
  ${PROJECT_SOURCE_DIR}/include/libdbc/dbc.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/message.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/signal.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/utils/utils.hpp
  ${PROJECT_SOURCE_DIR}/include/libdbc/exceptions/error.hpp
)

if(DBC_ENABLE_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

if(DBC_GENERATE_DOCS)
    add_subdirectory(doc)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE})
target_link_libraries(${PROJECT_NAME} FastFloat::fast_float)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

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
if (${CMAKE_MINOR_VERSION} GREATER_EQUAL 23 AND NOT FETCH_CONTENT_INCLUSION)
    install(TARGETS ${PROJECT_NAME}
                    FILE_SET HEADERS
                    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lib${PROJECT_NAME}
                    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
elseif(NOT DBC_FETCH_CONTENT_INCLUSION)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/libdbc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()


if (NOT DBC_FETCH_CONTENT_INCLUSION)
    # Generate pkg-config file
    configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
