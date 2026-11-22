#ifndef CSV_SAVE_FORMAT_HPP
#define CSV_SAVE_FORMAT_HPP

#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <optional>
#include <ostream>
#include <unordered_map>
#include <utility>

// key -> (total amount, number of rows)
using Summary = std::unordered_map<std::string, std::pair<double, size_t>>;

class CSV_save_format {

    public:
        CSV_save_format(const std::vector<double>& amounts,
                        const std::vector<std::string>& categories,
                        const std::vector<std::time_t>& timestamps);
        // Returns std::nullopt when column_name is not a column we can reduce by.
        std::optional<Summary> reduce(const std::string& column_name) const;

    private:
        std::vector<double> data_amount;
        std::vector<std::string> data_category;
        std::vector<std::time_t> data_timestamps;
};

#endif // CSV_SAVE_FORMAT_HPP
