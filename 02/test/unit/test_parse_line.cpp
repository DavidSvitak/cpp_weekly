#include "config.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <string_view>

using config_check::LineKind;
using config_check::parse_line;

TEST_CASE("parse_line parses and trims an assignment", "[parse_line]")
{
    const auto result = parse_line(" \tUSER_NAME = David Svitak\r");

    REQUIRE(result.kind == LineKind::entry);
    CHECK(result.key == "USER_NAME");
    CHECK(result.value == "David Svitak");
}

TEST_CASE("parse_line preserves internal value whitespace", "[parse_line]")
{
    const auto result = parse_line("MESSAGE=keep  internal   spaces");

    REQUIRE(result.kind == LineKind::entry);
    CHECK(result.key == "MESSAGE");
    CHECK(result.value == "keep  internal   spaces");
}

TEST_CASE("parse_line ignores blank and comment lines", "[parse_line]")
{
    CHECK(parse_line("").kind == LineKind::ignored);
    CHECK(parse_line(" \t\r").kind == LineKind::ignored);
    CHECK(parse_line("# comment").kind == LineKind::ignored);
    CHECK(parse_line("  # indented comment").kind == LineKind::ignored);
}

TEST_CASE("parse_line rejects malformed assignments", "[parse_line]")
{
    const std::array<std::string_view, 5> invalid_lines{
        "NO_SEPARATOR",
        "1INVALID=value",
        "BAD-KEY=value",
        "EMPTY_VALUE=",
        "TOO=MANY=EQUALS",
    };

    for (const auto line : invalid_lines) {
        CAPTURE(line);
        CHECK(parse_line(line).kind == LineKind::malformed);
    }
}