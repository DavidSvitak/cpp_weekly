#ifndef CSV_READER_HPP
#define CSV_READER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <ostream>
#include <algorithm>
#include "CSV_save_format.hpp"


class CSV_reader {
    public:
        CSV_reader(const std::string& input_path);
        // Both return 0 on success, 1 on error (unknown column, output file not writable).
        int save_to_csv(
            const std::string& output_filename,
            const std::string& reduction_column,
            const std::string& ordering_column
        ) const;
        int save_to_csv(
            std::ostream& out,
            const std::string& reduction_column,
            const std::string& ordering_column
        ) const;

    private:
        std::string filename;      // declared first, so it is also initialized first
        CSV_save_format csv_data;
};

#endif // CSV_READER_HPP
