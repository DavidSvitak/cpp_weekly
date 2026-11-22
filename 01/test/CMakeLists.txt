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
catch_discover_tests(csv_unit_tests TEST_PREFIX "01.")

# ---- End-to-end tests (data/*.csv vs data/*.expected) ----
# To be added together with cmake/run_cli_test.cmake.
