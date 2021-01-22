project(tests VERSION 0.1.0)

list(APPEND TEST_SOURCES main.cpp
			 test_file_loading.cpp
			 test_utils.cpp)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/third_party/Catch2/single_include)

add_executable(tests ${TEST_SOURCES} ${SOURCE})

add_custom_target(test
					COMMAND ${PROJECT_NAME}
					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
					DEPENDS ${PROJECT_NAME})