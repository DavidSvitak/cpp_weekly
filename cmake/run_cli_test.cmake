# Runs one command-line program and checks what it did. CTest runs it in script mode:
#
#   cmake -DPROGRAM=<exe> [-DARGS=<a;b;c>]
#         [-DEXPECTED_STDOUT=<file>] [-DEXPECTED_STDERR=<file>] [-DEXPECT_NO_STDERR=ON]
#         [-DEXPECTED_EXIT_CODE=<n>]
#         -P run_cli_test.cmake
#
# You normally do not call this directly: use add_cli_test() from CliTest.cmake.
#
# The test passes when every expectation that was given matches.
# message(FATAL_ERROR ...) makes cmake exit with a nonzero code, which CTest reports as a failure.

if(NOT DEFINED PROGRAM)
    message(FATAL_ERROR "run_cli_test.cmake: PROGRAM is required")
endif()
if(NOT DEFINED EXPECTED_EXIT_CODE)
    set(EXPECTED_EXIT_CODE 0)
endif()

execute_process(
    COMMAND "${PROGRAM}" ${ARGS}         # ${ARGS} unquoted: each list item becomes its own argument
    OUTPUT_VARIABLE actual_stdout
    ERROR_VARIABLE  actual_stderr
    RESULT_VARIABLE actual_exit_code
)

# Compare text with \n line endings everywhere, so the test also passes on Windows.
function(normalize_newlines var)
    string(REPLACE "\r\n" "\n" text "${${var}}")
    set(${var} "${text}" PARENT_SCOPE)
endfunction()

normalize_newlines(actual_stdout)
normalize_newlines(actual_stderr)

# Collect every mismatch, then report them all at once.
set(failures "")

if(NOT "${actual_exit_code}" STREQUAL "${EXPECTED_EXIT_CODE}")
    string(APPEND failures "exit code: expected ${EXPECTED_EXIT_CODE}, got ${actual_exit_code}\n")
endif()

if(EXPECT_NO_STDERR AND NOT actual_stderr STREQUAL "")
    string(APPEND failures
        "stderr: expected nothing\n"
        "--- actual ---\n${actual_stderr}"
        "----------------\n")
endif()

foreach(stream IN ITEMS STDOUT STDERR)
    if(DEFINED EXPECTED_${stream})
        string(TOLOWER "${stream}" lower)
        file(READ "${EXPECTED_${stream}}" expected)
        normalize_newlines(expected)
        if(NOT actual_${lower} STREQUAL expected)
            string(APPEND failures
                "${lower} does not match ${EXPECTED_${stream}}\n"
                "--- expected ---\n${expected}"
                "--- actual ---\n${actual_${lower}}"
                "----------------\n")
        endif()
    endif()
endforeach()

if(failures)
    message(FATAL_ERROR "Command: ${PROGRAM} ${ARGS}\n${failures}")
endif()
