#include <charconv>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <system_error>

#include "CSV_reader.hpp"

std::time_t parse_timestamp(const std::string& timestamp_str) {
    std::tm tm{};          // zero-initialize: get_time only fills year/month/day
    tm.tm_isdst = -1;      // let mktime work out daylight saving time
    std::istringstream ss(timestamp_str);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return std::mktime(&tm);
}

// Parses the whole string as a finite number. Rejects "12abc", " 5", "0x10", "", "nan", "inf", ...
std::optional<double> parse_amount(std::string_view s) {
    double value{};
    const char* end = s.data() + s.size();
    auto [ptr, ec] = std::from_chars(s.data(), end, value);
    if (ec != std::errc{} || ptr != end || !std::isfinite(value)) {
        return std::nullopt;
    }
    return value;
}

CSV_reader::CSV_reader(const std::string& input_path) : filename(input_path), csv_data({}, {}, {}) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // A constructor cannot return an error code, so report the failure by throwing.
        throw std::runtime_error("Could not open the file: " + filename);
    }
    std::vector<double> amounts;
    std::vector<std::string> categories;
    std::vector<std::time_t> timestamps;

    int i = 0;
    for (std::string line; std::getline(file, line); ) {
        i++;
        if (!line.empty() && line.back() == '\r') { // Windows line ending
            line.pop_back();
        }
        if (line == "date,category,amount") { // skip header
            continue;
        }
        if (line.starts_with("#")) { // Comment line, skip it
            continue;
        }
        std::istringstream ss(line);
        std::string timestamp_str, category, amount_str;
        if (!(std::getline(ss, timestamp_str, ',') &&
              std::getline(ss, category, ',') &&
              std::getline(ss, amount_str))) {
            std::cerr << "Warning: skipping malformed line " << i << std::endl;   // fewer than 3 fields
            continue;
        }
        std::optional<double> amount = parse_amount(amount_str);   // also rejects 4+ fields ("3,extra")
        if (!amount || category.empty() || timestamp_str.empty()) {
            std::cerr << "Warning: skipping malformed line " << i << std::endl;
            continue;
        }
        amounts.push_back(*amount);
        categories.push_back(category);
        timestamps.push_back(parse_timestamp(timestamp_str));
    }
    file.close();

    csv_data = CSV_save_format(amounts, categories, timestamps);
}

using SortedSummary = std::vector<std::pair<std::string, std::pair<double, size_t>>>;

// Returns std::nullopt when ordering_column is not "value" or "key".
std::optional<SortedSummary> order(const Summary& reduced_data, const std::string& ordering_column) {
    SortedSummary sorted_data(reduced_data.begin(), reduced_data.end());
    if (ordering_column == "value") {
        std::sort(sorted_data.begin(), sorted_data.end(), [](const auto& a, const auto& b) {
            if (a.second.first == b.second.first) {
                return a.first < b.first; // If values are equal, sort by key
            }
            return a.second.first > b.second.first;
        });
    } else if (ordering_column == "key") {
        std::sort(sorted_data.begin(), sorted_data.end(), [](const auto& a, const auto& b) {
            return a.first < b.first; // Sort by key
        });
    } else {
        std::cerr << "Invalid ordering column: " << ordering_column << std::endl;
        return std::nullopt;
    }
    return sorted_data;
}

int CSV_reader::save_to_csv(
    const std::string& output_filename,
    const std::string& reduction_column,
    const std::string& ordering_column) const {
    std::ofstream out(output_filename);
    if (!out.is_open()) {
        std::cerr << "Could not open the file for writing: " << output_filename << std::endl;
        return 1;
    }
    return save_to_csv(out, reduction_column, ordering_column);
}

int CSV_reader::save_to_csv(
    std::ostream& out,
    const std::string& reduction_column,
    const std::string& ordering_column) const {
    // Compute everything first, so nothing is printed if a column name is invalid.
    std::optional<Summary> reduced_data = csv_data.reduce(reduction_column);
    if (!reduced_data) {
        return 1;
    }
    std::optional<SortedSummary> sorted_data = order(*reduced_data, ordering_column);
    if (!sorted_data) {
        return 1;
    }

    out << reduction_column << ",value,count\n";
    out << std::fixed << std::setprecision(2);
    for (const auto& [key, value] : *sorted_data) {
        out << key << "," << value.first << "," << value.second << "\n";
    }
    return 0;
}
