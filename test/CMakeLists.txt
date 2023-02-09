enable_testing()

# Download and build Catch2 test framework
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.2.1
)
FetchContent_MakeAvailable(Catch2)
include(Catch)

add_executable(dbcParserTests test_dbc.cpp test_utils.cpp common.cpp)
target_compile_definitions(dbcParserTests PRIVATE TESTDBCFILES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dbcs")
target_link_libraries(dbcParserTests PRIVATE dbc Catch2::Catch2WithMain)
if (${CMAKE_MINOR_VERSION} GREATER_EQUAL 23)
    target_sources(dbcParserTests PRIVATE FILE_SET HEADERS
                                   TYPE HEADERS
                                   BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                                   FILES defines.hpp)
else()
    target_include_directories(dbcParserTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
catch_discover_tests(dbcParserTests)

add_executable(dbcParserParsemessageTests test_parseMessage.cpp common.cpp)
target_compile_definitions(dbcParserParsemessageTests PRIVATE TESTDBCFILES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dbcs")
target_link_libraries(dbcParserParsemessageTests PRIVATE dbc Catch2::Catch2WithMain)
if (${CMAKE_MINOR_VERSION} GREATER_EQUAL 23)
    target_sources(dbcParserParsemessageTests PRIVATE FILE_SET HEADERS
                                   TYPE HEADERS
                                   BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                                   FILES defines.hpp)
else()
    target_include_directories(dbcParserParsemessageTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
catch_discover_tests(dbcParserParsemessageTests)
