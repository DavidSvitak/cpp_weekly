#include <fstream>
#include <iostream>

#include "config.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: config_check <input.conf>";
        return 1;
    }

    std::string input = argv[1];
    std::ifstream input_stream(input);
    if (!input_stream) {
        std::cerr << "Error: cannot open " << "'" << input << "'";
        return 1;
    } 

    config_check::LoadResult result = config_check::load_config(input_stream);
    bool first = true;
    for (const auto& warn : result.warnings) {
        if (!first) {
            std::cerr << '\n';
        }
        std::cerr << warn;
        first = false;
    }
    std::cerr << std::flush;
    first = true;
    for (const auto& [key, value] : result.values) {
        if (!first) {
            std::cout << '\n';
        }
        std::cout << key << '=' << value;
        first = false;
    }
    std::cout << std::flush;
    return 0;
}
