cmake_minimum_required(VERSION 3.23)
# FILE_SET needs cmake 3.23

project(bitstream)

add_library(${PROJECT_NAME} STATIC bitstream.c)
target_sources(${PROJECT_NAME} INTERFACE FILE_SET HEADERS
	TYPE HEADERS
	BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES bitstream.h)
