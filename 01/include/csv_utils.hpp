#ifndef CSV_UTILS_HPP
#define CSV_UTILS_HPP

#include <ctime>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "CSV_save_format.hpp"

// Free helper functions used by CSV_reader. They are declared here (instead of only
// existing inside CSV_reader.cpp) so that unit tests can call them directly.

// (key, (total, count)) pairs in output order.
using SortedSummary = std::vector<std::pair<std::string, std::pair<double, size_t>>>;

// Parses the whole string as a finite number. Rejects "12abc", " 5", "0x10", "", "nan", "inf", ...
std::optional<double> parse_amount(std::string_view s);

// Parses a "YYYY-MM-DD" date into a time_t (local time).
std::time_t parse_timestamp(const std::string& timestamp_str);

// Sorts a summary. "value": total descending, ties by key ascending. "key": key ascending.
// Returns std::nullopt for any other ordering_column.
std::optional<SortedSummary> order(const Summary& reduced_data, const std::string& ordering_column);

#endif // CSV_UTILS_HPP
