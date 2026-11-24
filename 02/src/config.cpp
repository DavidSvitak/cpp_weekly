#include "config.hpp"

namespace config_check {

std::string_view trim(std::string_view s) {
    const auto first = s.find_first_not_of(" \t\r");
    if (first == std::string_view::npos) return {};   // empty or all whitespace
    const auto last = s.find_last_not_of(" \t\r");
    return s.substr(first, last - first + 1);         // second arg is a LENGTH
}

bool is_invalid_char(char c) {
    const unsigned char u = static_cast<unsigned char>(c);
    if ((u >= 48 && u <= 57)  ||    // '0'..'9'
        (u >= 65 && u <= 90)  ||    // 'A'..'Z'
        (u >= 97 && u <= 122) ||    // 'a'..'z'
        (u == 95)) {                // _
        return false;
    }
    return true;
}

ParsedLine parse_line(std::string_view line) {
    ParsedLine result;
    // Trim whitespace from the beggining and the end of the line
    line = trim(line);
    if (line.empty() || line.starts_with("#")) { // ignore comments and empty lines
        result.kind = LineKind::ignored;
        return result;
    } else {
        std::vector<std::size_t> equal_sign_positions;
        // Start at index 1 on purpose: a line starting with '=' has an empty key
        // and is malformed either way, so it is fine if that '=' isn't counted.
        std::size_t res = 0;
        while ((res = line.find('=', res + 1)) != std::string_view::npos) {
            equal_sign_positions.push_back(res);
        }
        if (equal_sign_positions.size() == 1) {
            std::size_t equal_pos = equal_sign_positions[0];
            result.key = trim(line.substr(0, equal_pos));
            result.value = trim(line.substr(equal_pos+1));
            if (result.value.empty() || result.key.empty()) { //get rid of empty values
                result.kind = LineKind::malformed;
                return result;
            }
        } else { // Flag lines with no or multiple equal signs
            result.kind = LineKind::malformed;
            return result;
        }
        for (auto c: result.key) { // check for non letter and diggit characters except '_'
            if (is_invalid_char(c)) {
                result.kind = LineKind::malformed;
                return result;
            }
        }
        if (isdigit(result.key[0])) { // if the key starts with a diggit
            result.kind = LineKind::malformed;
            return result;
        }
        result.kind = LineKind::entry;
        return result;
    }
}

LoadResult load_config(std::istream& input) {
    LoadResult result;
    std::string line;
    std::size_t i = 0;
    while (std::getline(input, line)) {
        i++;
        auto parsed = parse_line(line);
        if (parsed.kind == LineKind::entry) {
            if (result.values.contains(parsed.key)) {
                result.warnings.push_back("Warning: duplicate key '" + parsed.key + "' on line " + std::to_string(i) + "; replacing previous value");
            }
            result.values[parsed.key] = parsed.value;
        } else if (parsed.kind == LineKind::malformed) {
            result.warnings.push_back("Warning: skipping malformed line " + std::to_string(i));
        }
    }
    return result;
}


}