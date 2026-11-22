#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <string>
#include <vector>

#include "args.hpp"

namespace {

// Builds an argv-like array from strings and calls parse_args.
// The first element plays the role of the program name, like argv[0].
// `args` is taken by value so the strings live until parse_args returns,
// and so .data() gives the non-const char* that argv contains.
std::optional<Options> parse(std::vector<std::string> args) {
    std::vector<char*> argv;
    for (std::string& arg : args) {
        argv.push_back(arg.data());
    }
    return parse_args(argv);   // std::vector<char*> converts to std::span<char* const>
}

} // namespace

TEST_CASE("parse_args with only an input file uses the defaults", "[args]") {
    auto opts = parse({"csv_summary", "expenses.csv"});

    REQUIRE(opts.has_value());   // REQUIRE stops the test here if it fails, so *opts below is safe
    CHECK(opts->input == "expenses.csv");
    CHECK(opts->order_col == "value");
    CHECK(opts->reduce_col == "Category");
    CHECK_FALSE(opts->output.has_value());   // no -o means standard output
}

TEST_CASE("parse_args reads each option's value", "[args]") {
    SECTION("-o sets the output file") {
        auto opts = parse({"csv_summary", "in.csv", "-o", "out.csv"});
        REQUIRE(opts.has_value());
        CHECK(opts->output == "out.csv");
    }
    SECTION("-r sets the ordering column") {
        auto opts = parse({"csv_summary", "in.csv", "-r", "key"});
        REQUIRE(opts.has_value());
        CHECK(opts->order_col == "key");
    }
    SECTION("-d sets the reduction column") {
        auto opts = parse({"csv_summary", "in.csv", "-d", "Timestamp"});
        REQUIRE(opts.has_value());
        CHECK(opts->reduce_col == "Timestamp");
    }
}

TEST_CASE("parse_args accepts options before and after the input file", "[args]") {
    auto opts = parse({"csv_summary", "-r", "key", "in.csv", "-o", "out.csv"});

    REQUIRE(opts.has_value());
    CHECK(opts->input == "in.csv");
    CHECK(opts->order_col == "key");
    CHECK(opts->output == "out.csv");
}

// README rule 1 and rule 8: exactly one argument, otherwise a nonzero exit code.
// parse_args signals "invalid" by returning std::nullopt; main turns that into exit code 1.
TEST_CASE("parse_args rejects a missing input file", "[args]") {
    CHECK_FALSE(parse({"csv_summary"}).has_value());
    CHECK_FALSE(parse({"csv_summary", "-r", "key"}).has_value());
}

TEST_CASE("parse_args rejects more than one input file", "[args]") {
    CHECK_FALSE(parse({"csv_summary", "a.csv", "b.csv"}).has_value());
}

TEST_CASE("parse_args rejects unknown options", "[args]") {
    CHECK_FALSE(parse({"csv_summary", "in.csv", "-x"}).has_value());
    CHECK_FALSE(parse({"csv_summary", "in.csv", "--output", "out.csv"}).has_value());
}

TEST_CASE("parse_args rejects an option without its value", "[args]") {
    CHECK_FALSE(parse({"csv_summary", "in.csv", "-o"}).has_value());
    CHECK_FALSE(parse({"csv_summary", "in.csv", "-r"}).has_value());
    CHECK_FALSE(parse({"csv_summary", "in.csv", "-d"}).has_value());
}
