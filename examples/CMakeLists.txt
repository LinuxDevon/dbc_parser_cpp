project(examples)
    
set(EXECUTABLE_EXAMPLES "")

function(add_example_executable SOURCE_FILE)
    set(SOURCE_NAME "")
    string(REPLACE "src/example_" "" SOURCE_NAME ${SOURCE_FILE})
    string(REPLACE ".cpp" "" SOURCE_NAME ${SOURCE_NAME})

    add_executable(${SOURCE_NAME} ${SOURCE_FILE})
    target_link_libraries(${SOURCE_NAME} PRIVATE dbc)
	target_compile_definitions(${SOURCE_NAME} PRIVATE DBC_FILE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dbcs")

    set(EXECUTABLE_EXAMPLES "examples/${SOURCE_NAME} ${EXECUTABLE_EXAMPLES} " PARENT_SCOPE)
endfunction()

if(DBC_BUILD_EXAMPLES)
    add_example_executable(src/example_read_simple_dbc.cpp)
    add_example_executable(src/example_parse_message.cpp)

    string(STRIP ${EXECUTABLE_EXAMPLES} EXECUTABLE_EXAMPLES)

    add_custom_target(test_examples ALL
                        DEPENDS "${EXECUTABLE_EXAMPLES}"
						WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
						COMMAND ${PROJECT_SOURCE_DIR}/validate_examples.py
	)
endif()
