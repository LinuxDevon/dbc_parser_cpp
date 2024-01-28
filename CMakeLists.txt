cmake_minimum_required(VERSION 3.16)

project(dbc
    VERSION 0.2.0
    DESCRIPTION "C++ DBC Parser"
)

# -- PROJECT OPTIONS  -- #
option(DBC_ENABLE_TESTS "Enable Unittests" ON)
option(DBC_TEST_LOCALE_INDEPENDENCE "Used to deterime if the libary is locale agnostic when it comes to converting floats. You need `de_DE.UTF-8` locale installed for this testing." OFF)
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

# add where to find the source files
list(APPEND SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
	${PROJECT_SOURCE_DIR}/src/message.cpp
	${PROJECT_SOURCE_DIR}/src/signal.cpp
	${PROJECT_SOURCE_DIR}/src/dbc.cpp
)

list(APPEND HEADER_FILES
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

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} FastFloat::fast_float)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

target_sources(${PROJECT_NAME} INTERFACE FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include/libdbc
        FILES ${HEADER_FILES}
)

add_custom_target(release
					WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					COMMAND ${CMAKE_SOURCE_DIR}/scripts/create_single_header.sh
					DEPENDS ${PROJECT_NAME}
)

## Installation
# install lib
install(TARGETS ${PROJECT_NAME}
		DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/libdbc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Generate pkg-config file
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
