#include "config.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>
#include <vector>

using config_check::load_config;

TEST_CASE("load_config stores valid assignments", "[load_config]")
{
    std::istringstream input{
        "PORT=8080\n"
        "HOST = localhost\n"
    };

    const auto result = load_config(input);

    REQUIRE(result.values.size() == 2);
    CHECK(result.values.at("HOST") == "localhost");
    CHECK(result.values.at("PORT") == "8080");
    CHECK(result.warnings.empty());
}

TEST_CASE("load_config replaces duplicates and records warnings", "[load_config]")
{
    std::istringstream input{
        "PORT=8080\n"
        "PORT=9090\n"
        "BROKEN\n"
        "# ignored\n"
        "MODE=development\n"
    };

    const auto result = load_config(input);

    REQUIRE(result.values.size() == 2);
    CHECK(result.values.at("PORT") == "9090");
    CHECK(result.values.at("MODE") == "development");

    const std::vector<std::string> expected_warnings{
        "Warning: duplicate key 'PORT' on line 2; replacing previous value",
        "Warning: skipping malformed line 3",
    };

    CHECK(result.warnings == expected_warnings);
}

TEST_CASE("load_config ignores comments and empty lines without warnings",
          "[load_config]")
{
    std::istringstream input{
        "\n"
        "# comment\n"
        "  \t\n"
        "NAME=value\n"
    };

    const auto result = load_config(input);

    REQUIRE(result.values.size() == 1);
    CHECK(result.values.at("NAME") == "value");
    CHECK(result.warnings.empty());
}