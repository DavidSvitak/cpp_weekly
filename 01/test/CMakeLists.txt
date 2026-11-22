# ---- Unit tests (Catch2) ----
add_executable(csv_unit_tests
    unit/test_args.cpp
    unit/test_parse_amount.cpp
    unit/test_summary.cpp
)
# Catch2WithMain provides main(), so the test files contain only test cases.
target_link_libraries(csv_unit_tests PRIVATE csv_lib Catch2::Catch2WithMain cpp_weekly_warnings)

# Registers every TEST_CASE as its own CTest test, named "01.<test case name>".
# Run only this week with:  ctest --test-dir build -R "^01\."
catch_discover_tests(csv_unit_tests
    TEST_PREFIX "01."
    PROPERTIES LABELS "unit"
)

# ---- End-to-end tests: run csv_summary and check stdout, stderr and the exit code ----
# All of them run inside data/, so file names can be written without a path.
set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

# One test per data/<name>.csv, comparing stdout with data/<name>.expected.
# If data/<name>.expected_err exists, stderr must match it; otherwise stderr must be empty.
# To add a case, drop a new .csv + .expected pair into data/ (CONFIGURE_DEPENDS re-checks the
# folder on every build, so no edit is needed here).
file(GLOB csv_cases CONFIGURE_DEPENDS "${DATA_DIR}/*.csv")
foreach(csv IN LISTS csv_cases)
    cmake_path(GET csv STEM name)                      # data/tie_break.csv -> tie_break

    if(EXISTS "${DATA_DIR}/${name}.expected_err")
        set(stderr_check EXPECTED_STDERR "${DATA_DIR}/${name}.expected_err")
    else()
        set(stderr_check NO_STDERR)
    endif()

    add_cli_test(
        NAME 01.e2e.${name}
        PROGRAM csv_summary
        ARGS ${name}.csv
        WORKING_DIRECTORY "${DATA_DIR}"
        EXPECTED_STDOUT "${DATA_DIR}/${name}.expected"
        ${stderr_check}
    )
endforeach()

# Options that change the output.
add_cli_test(
    NAME 01.e2e.order_by_key
    PROGRAM csv_summary
    ARGS tie_break.csv -r key
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_STDOUT "${DATA_DIR}/tie_break.by_key.expected"
    NO_STDERR
)

# README rule 8: return a nonzero exit code if the argument is missing, there are too many
# arguments, or the file cannot be opened.
add_cli_test(
    NAME 01.e2e.no_arguments_fails
    PROGRAM csv_summary
    EXPECTED_EXIT_CODE 1
)
add_cli_test(
    NAME 01.e2e.too_many_arguments_fails
    PROGRAM csv_summary
    ARGS readme_example.csv tie_break.csv
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_EXIT_CODE 1
)
add_cli_test(
    NAME 01.e2e.missing_file_fails
    PROGRAM csv_summary
    ARGS does_not_exist.csv
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_EXIT_CODE 1
)

# Invalid option values also fail, and print nothing to stdout.
add_cli_test(
    NAME 01.e2e.invalid_order_column_fails
    PROGRAM csv_summary
    ARGS readme_example.csv -r total
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_STDOUT "${DATA_DIR}/empty.expected"
    EXPECTED_EXIT_CODE 1
)
add_cli_test(
    NAME 01.e2e.invalid_reduce_column_fails
    PROGRAM csv_summary
    ARGS readme_example.csv -d amount
    WORKING_DIRECTORY "${DATA_DIR}"
    EXPECTED_STDOUT "${DATA_DIR}/empty.expected"
    EXPECTED_EXIT_CODE 1
)
