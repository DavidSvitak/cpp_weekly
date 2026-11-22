cmake_minimum_required(VERSION 3.20)

project(cpp_weekly LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Shared compiler warnings. Each week links this to its own targets:
#   target_link_libraries(my_target PRIVATE cpp_weekly_warnings)
# It is linked per target (not set globally), so third-party code like Catch2 is not affected.
add_library(cpp_weekly_warnings INTERFACE)
if(MSVC)
    target_compile_options(cpp_weekly_warnings INTERFACE /W4 /permissive-)
else()
    target_compile_options(cpp_weekly_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow)
endif()

# OFF locally (a warning should not stop you while experimenting), ON in CI (a warning fails the build).
option(CPP_WEEKLY_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
if(CPP_WEEKLY_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(cpp_weekly_warnings INTERFACE /WX)
    else()
        target_compile_options(cpp_weekly_warnings INTERFACE -Werror)
    endif()
endif()

# Testing. include(CTest) calls enable_testing() and adds the BUILD_TESTING option (ON by default),
# so `cmake -DBUILD_TESTING=OFF` builds only the programs.
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.7.1      # pinned: the build never changes on its own
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)
    # Provides catch_discover_tests(), which registers every TEST_CASE with CTest.
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(Catch)

    # Provides add_cli_test(), for end-to-end tests that run a program and check its output.
    include(cmake/CliTest.cmake)
endif()

add_subdirectory(01)
