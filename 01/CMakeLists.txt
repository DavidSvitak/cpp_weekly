add_executable(csv_summary
    src/main.cpp
    src/args.cpp
    src/CSV_reader.cpp
    src/CSV_save_format.cpp
)

set_target_properties(csv_summary PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_compile_features(csv_summary PRIVATE cxx_std_20)

target_include_directories(csv_summary PRIVATE include)

# Compiler warnings: /W4 for MSVC, the usual -W set for GCC and Clang.
if(MSVC)
    target_compile_options(csv_summary PRIVATE /W4 /permissive-)
else()
    target_compile_options(csv_summary PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
endif()