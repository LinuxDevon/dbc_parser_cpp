enable_testing()

# Download and build Catch2 test framework
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)
include(Catch)

# Need filesystem for testing
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Code coverage compiler specific
if (GCC)
  add_compile_options(--coverage)
endif()

add_executable(dbcParserTests
    test_dbc.cpp
    test_utils.cpp
    test_parseMessage.cpp
    common.cpp
)

target_compile_definitions(dbcParserTests PRIVATE TESTDBCFILES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dbcs")
target_link_libraries(dbcParserTests PRIVATE dbc Catch2::Catch2WithMain)
target_include_directories(dbcParserTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(dbcParserTests)

# We want a seperate binary for this test. We setup global locals which mess with all of the testing.
# Opting for a sperate test running so we don't conflict
if(DBC_TEST_LOCALE_INDEPENDENCE)
  add_executable(dbcLocaleTests
      locale_testing/test_locale_main.cpp
      common.cpp
  )

  target_compile_definitions(dbcLocaleTests PRIVATE TESTDBCFILES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dbcs")
  target_link_libraries(dbcLocaleTests PRIVATE dbc Catch2::Catch2WithMain)
  target_include_directories(dbcLocaleTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  catch_discover_tests(dbcLocaleTests)
else()
  message(WARNING "Locale independent testing is turned off!")
endif()
