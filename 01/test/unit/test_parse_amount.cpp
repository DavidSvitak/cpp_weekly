#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>

#include "csv_utils.hpp"

// README rule 7: "A row is malformed when its amount is not entirely numeric."

TEST_CASE("parse_amount accepts plain numbers", "[parse_amount]") {
    // These values are exactly representable as double, so == is safe here.
    REQUIRE(parse_amount("42") == 42.0);
    REQUIRE(parse_amount("12.5") == 12.5);
    REQUIRE(parse_amount("0") == 0.0);
    REQUIRE(parse_amount("0.25") == 0.25);
}

TEST_CASE("parse_amount accepts negative numbers", "[parse_amount]") {
    // A refund or correction is still a valid number.
    REQUIRE(parse_amount("-5.25") == -5.25);
}

TEST_CASE("parse_amount rejects anything that is not entirely a number", "[parse_amount]") {
    // GENERATE runs the rest of this test case once for every value in the list.
    auto input = GENERATE(as<std::string>{},
        "",           // empty field
        "abc",        // no digits at all
        "12abc",      // trailing letters (std::stof would have accepted this as 12)
        " 5",         // leading space
        "5 ",         // trailing space
        "3,extra",    // a 4th column ends up in the amount field
        "0x10",       // hexadecimal
        "1e999"       // too large for a double
    );
    CAPTURE(input);   // if the check fails, Catch2 prints which input it was
    REQUIRE_FALSE(parse_amount(input).has_value());
}

TEST_CASE("parse_amount rejects nan and infinity in any spelling", "[parse_amount]") {
    auto input = GENERATE(as<std::string>{},
        "nan", "NaN", "NAN", "inf", "-inf", "INF", "Infinity"
    );
    CAPTURE(input);
    REQUIRE_FALSE(parse_amount(input).has_value());
}
