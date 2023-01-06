project(tests VERSION 0.1.0)

# Download and build Catch2 test framework
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.2.1
)
FetchContent_MakeAvailable(Catch2)

list(APPEND TEST_SOURCES
			 test_dbc.cpp
			 test_utils.cpp)

add_executable(tests ${TEST_SOURCES} ${SOURCE})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

add_custom_target(test
					COMMAND ${PROJECT_NAME}
					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
					DEPENDS ${PROJECT_NAME})