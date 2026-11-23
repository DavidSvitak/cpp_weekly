add_library(config_lib STATIC
    src/config.cpp
)

target_include_directories(config_lib PUBLIC include)
target_compile_features(config_lib PUBLIC cxx_std_20)
target_link_libraries(config_lib PRIVATE cpp_weekly_warnings)

add_executable(config_check
    src/main.cpp
)

target_link_libraries(config_check PRIVATE
    config_lib
    cpp_weekly_warnings
)

set_target_properties(config_check PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()