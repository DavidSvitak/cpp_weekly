#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <istream>
#include <map>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

namespace config_check {

enum class LineKind {
    ignored,
    entry,
    malformed,
};

struct ParsedLine {
    LineKind kind;
    std::string key;
    std::string value;
};

ParsedLine parse_line(std::string_view line);

struct LoadResult {
    std::map<std::string, std::string> values;
    std::vector<std::string> warnings;
};

LoadResult load_config(std::istream& input);

}  // namespace config_check

#endif  // CONFIG_HPP