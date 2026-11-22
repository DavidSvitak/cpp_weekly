#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>
#include <optional>
#include <span>

struct Options {
    std::string input;
    std::string order_col  = "value";
    std::string reduce_col = "Category";
    std::optional<std::string> output;   // empty → stdout
};

std::optional<Options> parse_args(std::span<char* const> args);

#endif // ARGS_HPP