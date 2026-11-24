# ---- Unit tests ----

add_executable(config_unit_tests
    unit/test_load_config.cpp
    unit/test_parse_line.cpp
)

target_link_libraries(config_unit_tests PRIVATE
    config_lib
    Catch2::Catch2WithMain
    cpp_weekly_warnings
)

catch_discover_tests(config_unit_tests
    TEST_PREFIX "02."
    PROPERTIES LABELS "unit"
)

# ---- End-to-end tests ----

set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

file(GLOB config_cases CONFIGURE_DEPENDS "${DATA_DIR}/*.conf")

foreach(config_file IN LISTS config_cases)
    cmake_path(GET config_file STEM name)

    if(EXISTS "${DATA_DIR}/${name}.expected_err")
        set(stderr_check
            EXPECTED_STDERR "${DATA_DIR}/${name}.expected_err"
        )
    else()
        set(stderr_check NO_STDERR)
    endif()

    add_cli_test(
        NAME 02.e2e.${name}
        PROGRAM config_check
        ARGS ${name}.conf
        WORKING_DIRECTORY "${DATA_DIR}"
        EXPECTED_STDOUT "${DATA_DIR}/${name}.expected"
        ${stderr_check}
    )
endforeach()

add_cli_test(
    NAME 02.e2e.no_arguments_fails
    PROGRAM config_check
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_STDOUT "${DATA_DIR}/empty.expected"
    EXPECTED_STDERR "${DATA_DIR}/usage.expected_err"
    EXPECTED_EXIT_CODE 1
)

add_cli_test(
    NAME 02.e2e.too_many_arguments_fail
    PROGRAM config_check
    ARGS normal.conf extra.conf
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_STDOUT "${DATA_DIR}/empty.expected"
    EXPECTED_STDERR "${DATA_DIR}/usage.expected_err"
    EXPECTED_EXIT_CODE 1
)

add_cli_test(
    NAME 02.e2e.missing_file_fails
    PROGRAM config_check
    ARGS does_not_exist.conf
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_STDOUT "${DATA_DIR}/empty.expected"
    EXPECTED_STDERR "${DATA_DIR}/missing_file.expected_err"
    EXPECTED_EXIT_CODE 1
)