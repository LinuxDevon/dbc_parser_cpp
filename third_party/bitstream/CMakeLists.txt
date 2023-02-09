cmake_minimum_required(VERSION 3.16)
# FILE_SET needs cmake 3.23

project(bitstream)

add_library(${PROJECT_NAME} STATIC bitstream.c)

if (${CMAKE_MINOR_VERSION} GREATER_EQUAL 23)
    target_sources(${PROJECT_NAME} INTERFACE FILE_SET HEADERS
            TYPE HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}
            FILES bitstream.h)
else()
    target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
