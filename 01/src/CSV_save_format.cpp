#include "CSV_save_format.hpp"

CSV_save_format::CSV_save_format(
    const std::vector<double>& amounts,
    const std::vector<std::string>& categories,
    const std::vector<std::time_t>& timestamps)
    : data_amount(amounts), data_category(categories), data_timestamps(timestamps) {
}

std::string format_timestamp(const std::time_t& timestamp) {
    char buffer[256];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&timestamp));
    return std::string(buffer);
}

std::optional<Summary> CSV_save_format::reduce(const std::string& column_name) const {
    Summary summary;
    if (column_name == "Category") {
        for (size_t i = 0; i < data_amount.size(); ++i) {
            auto& [total, count] = summary[data_category[i]];   // inserts {0.0, 0} if missing
            total += data_amount[i];
            count += 1;
        }
    } else if (column_name == "Timestamp") {
        for (size_t i = 0; i < data_amount.size(); ++i) {
            auto& [total, count] = summary[format_timestamp(data_timestamps[i])];
            total += data_amount[i];
            count += 1;
        }
    } else {
        std::cerr << "Invalid column name for reduction: " << column_name << std::endl;
        return std::nullopt;
    }
    return summary;
}
