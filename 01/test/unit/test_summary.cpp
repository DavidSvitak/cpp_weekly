#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <ctime>
#include <string>
#include <vector>

#include "CSV_save_format.hpp"
#include "csv_utils.hpp"

using Catch::Matchers::WithinAbs;

// README rule 3: for each category, calculate the number of expenses and the total amount.

TEST_CASE("reduce by Category sums totals and counts rows", "[reduce]") {
    std::time_t day = parse_timestamp("2024-01-01");
    CSV_save_format data(
        {10.10, 20.20, 5.00, 0.70},
        {"Food", "Food", "Rent", "Food"},
        {day, day, day, day}
    );

    auto summary = data.reduce("Category");

    REQUIRE(summary.has_value());
    REQUIRE(summary->size() == 2);
    // 10.10 + 20.20 + 0.70 is not exact in binary floating point,
    // so compare with a tolerance instead of ==.
    CHECK_THAT(summary->at("Food").first, WithinAbs(31.00, 1e-9));
    CHECK(summary->at("Food").second == 3);
    CHECK_THAT(summary->at("Rent").first, WithinAbs(5.00, 1e-9));
    CHECK(summary->at("Rent").second == 1);
}

TEST_CASE("reduce by Timestamp groups rows from the same day", "[reduce]") {
    CSV_save_format data(
        {1.0, 2.0, 4.0},
        {"Food", "Rent", "Food"},
        {parse_timestamp("2024-01-01"), parse_timestamp("2024-01-01"), parse_timestamp("2024-01-02")}
    );

    auto summary = data.reduce("Timestamp");

    REQUIRE(summary.has_value());
    REQUIRE(summary->size() == 2);
    CHECK(summary->at("2024-01-01") == std::pair<double, size_t>{3.0, 2});
    CHECK(summary->at("2024-01-02") == std::pair<double, size_t>{4.0, 1});
}

TEST_CASE("reduce on no rows gives an empty summary", "[reduce]") {
    CSV_save_format data({}, {}, {});

    auto summary = data.reduce("Category");

    REQUIRE(summary.has_value());   // empty is valid: a header-only file has no rows
    CHECK(summary->empty());
}

TEST_CASE("reduce rejects an unknown column", "[reduce]") {
    CSV_save_format data({}, {}, {});
    CHECK_FALSE(data.reduce("category").has_value());   // names are case-sensitive
    CHECK_FALSE(data.reduce("Amount").has_value());
}

// README rule 4: sort by total from largest to smallest; alphabetically when totals are equal.

TEST_CASE("order by value sorts totals from largest to smallest", "[order]") {
    Summary summary{
        {"Food", {30.0, 2}},
        {"Rent", {500.0, 1}},
        {"Fun",  {12.5, 3}},
    };

    auto sorted = order(summary, "value");

    REQUIRE(sorted.has_value());
    REQUIRE(sorted->size() == 3);
    CHECK((*sorted)[0].first == "Rent");
    CHECK((*sorted)[1].first == "Food");
    CHECK((*sorted)[2].first == "Fun");
}

TEST_CASE("order by value breaks ties alphabetically, whatever the counts", "[order]") {
    // Same total, different counts: the tie-break must look only at the total.
    Summary summary{
        {"Zoo", {20.0, 1}},
        {"Gym", {20.0, 2}},
        {"Art", {20.0, 2}},
        {"Bar", {50.0, 1}},
    };

    auto sorted = order(summary, "value");

    REQUIRE(sorted.has_value());
    std::vector<std::string> keys;
    for (const auto& [key, value] : *sorted) {
        keys.push_back(key);
    }
    CHECK(keys == std::vector<std::string>{"Bar", "Art", "Gym", "Zoo"});
}

TEST_CASE("order by key sorts alphabetically", "[order]") {
    Summary summary{
        {"Rent", {500.0, 1}},
        {"Art",  {1.0, 1}},
        {"Food", {30.0, 2}},
    };

    auto sorted = order(summary, "key");

    REQUIRE(sorted.has_value());
    CHECK((*sorted)[0].first == "Art");
    CHECK((*sorted)[1].first == "Food");
    CHECK((*sorted)[2].first == "Rent");
}

TEST_CASE("order rejects an unknown ordering column", "[order]") {
    Summary summary{{"Food", {1.0, 1}}};
    CHECK_FALSE(order(summary, "total").has_value());
}
