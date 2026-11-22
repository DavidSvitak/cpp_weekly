# All program logic except main() lives in a library, so the program and the tests
# are built from exactly the same code.
add_library(csv_lib STATIC
    src/args.cpp
    src/CSV_reader.cpp
    src/CSV_save_format.cpp
)
# PUBLIC: anything that links csv_lib (the program, the tests) also gets the
# include directory and C++20.
target_include_directories(csv_lib PUBLIC include)
target_compile_features(csv_lib PUBLIC cxx_std_20)
target_link_libraries(csv_lib PRIVATE cpp_weekly_warnings)

add_executable(csv_summary src/main.cpp)
target_link_libraries(csv_summary PRIVATE csv_lib cpp_weekly_warnings)
set_target_properties(csv_summary PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
