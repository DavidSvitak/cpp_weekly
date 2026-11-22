# add_cli_test(): registers an end-to-end test that runs a program and checks its output.
#
#   add_cli_test(
#       NAME <test name>
#       PROGRAM <target>                      # an add_executable() target, e.g. csv_summary
#       [ARGS <arg>...]                       # arguments passed to the program
#       [WORKING_DIRECTORY <dir>]             # where it runs; default: the current source dir
#       [EXPECTED_STDOUT <file>]              # stdout must equal this file exactly
#       [EXPECTED_STDERR <file>]              # stderr must equal this file exactly
#       [NO_STDERR]                           # stderr must be empty
#       [EXPECTED_EXIT_CODE <n>]              # default 0
#   )
#
# Relative file paths are resolved against the current source directory (the folder of the
# CMakeLists.txt that calls this). The checking itself is done by run_cli_test.cmake.

# Remember where run_cli_test.cmake is. CMAKE_CURRENT_LIST_DIR is this file's folder,
# but only while this file is being included, so it is saved now.
set(_CLI_TEST_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/run_cli_test.cmake")

function(add_cli_test)
    cmake_parse_arguments(PARSE_ARGV 0 arg
        "NO_STDERR"                                                                  # flags
        "NAME;PROGRAM;WORKING_DIRECTORY;EXPECTED_STDOUT;EXPECTED_STDERR;EXPECTED_EXIT_CODE"  # one value
        "ARGS"                                                                       # many values
    )
    if(NOT arg_NAME OR NOT arg_PROGRAM)
        message(FATAL_ERROR "add_cli_test: NAME and PROGRAM are required")
    endif()
    if(arg_NO_STDERR AND arg_EXPECTED_STDERR)
        message(FATAL_ERROR "add_cli_test(${arg_NAME}): use NO_STDERR or EXPECTED_STDERR, not both")
    endif()
    if(NOT arg_WORKING_DIRECTORY)
        set(arg_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    # $<TARGET_FILE:...> is the real path of the built executable (including .exe on Windows).
    set(script_args "-DPROGRAM=$<TARGET_FILE:${arg_PROGRAM}>")

    if(arg_ARGS)
        # add_test() would split a plain ";" into separate cmake arguments, and "\;" reaches the
        # script with the backslashes still in it. $<SEMICOLON> becomes ";" only after the
        # splitting, so the script receives one -DARGS=a;b;c, which is a proper CMake list.
        list(JOIN arg_ARGS "$<SEMICOLON>" joined)
        list(APPEND script_args "-DARGS=${joined}")
    endif()

    foreach(stream IN ITEMS STDOUT STDERR)
        if(arg_EXPECTED_${stream})
            cmake_path(ABSOLUTE_PATH arg_EXPECTED_${stream} NORMALIZE)
            list(APPEND script_args "-DEXPECTED_${stream}=${arg_EXPECTED_${stream}}")
        endif()
    endforeach()

    if(arg_NO_STDERR)
        list(APPEND script_args "-DEXPECT_NO_STDERR=ON")
    endif()
    if(DEFINED arg_EXPECTED_EXIT_CODE)
        list(APPEND script_args "-DEXPECTED_EXIT_CODE=${arg_EXPECTED_EXIT_CODE}")
    endif()

    add_test(
        NAME ${arg_NAME}
        COMMAND ${CMAKE_COMMAND} ${script_args} -P "${_CLI_TEST_SCRIPT}"
        WORKING_DIRECTORY "${arg_WORKING_DIRECTORY}"
    )
    set_tests_properties(${arg_NAME} PROPERTIES LABELS "e2e")
endfunction()
