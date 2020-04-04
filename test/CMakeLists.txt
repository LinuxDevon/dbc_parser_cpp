project(tests VERSION 0.1.0)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/third_party/Catch2/single_include)

add_executable(tests main.cpp file_loading.cpp)

add_custom_target(test
					COMMAND ${PROJECT_NAME}
					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
					DEPENDS ${PROJECT_NAME})