#include "args.hpp"

#include <string_view>
#include <vector>

std::optional<Options> parse_args(std::span<char* const> args) {
    Options options;
    std::vector<std::string> positional;

    // Skip args[0] (program name). Options may appear before or after the input file.
    for (size_t i = 1; i < args.size(); ++i) {
        std::string_view arg = args[i];
        bool has_value = i + 1 < args.size();

        if (arg == "-o" || arg == "-r" || arg == "-d") {
            if (!has_value) {
                return std::nullopt; // Option is missing its value
            }
            std::string value = args[++i];
            if (arg == "-o") {
                options.output = value;
            } else if (arg == "-r") {
                options.order_col = value;
            } else {
                options.reduce_col = value;
            }
        } else if (arg.starts_with("-") && arg != "-") {
            return std::nullopt; // Unknown option
        } else {
            positional.emplace_back(arg);
        }
    }

    if (positional.size() != 1) {
        return std::nullopt; // Missing input file, or too many arguments
    }
    options.input = positional.front();

    return options;
}
