#include <cstdlib>
#include <exception>
#include <iostream>
#include <span>
#include <optional>

#include "CSV_save_format.hpp"
#include "CSV_reader.hpp"
#include "args.hpp"

int main(int argc, char* argv[]) {
  auto opts = parse_args(std::span(argv, argc));
  if (!opts) {
    std::cerr << "Usage: " << argv[0]
              << " <input.csv> [-o OUTPUT_FILE] [-r ORDER_COL] [-d REDUCE_COL]\n";
    return EXIT_FAILURE;
  }

  try {
    CSV_reader reader(opts->input);
    if (opts->output.value_or("") == "") {
      int result = reader.save_to_csv(
        std::cout,
        opts->reduce_col,
        opts->order_col
      );
      return result;
    } else {
      int result = reader.save_to_csv(
        opts->output.value(),
        opts->reduce_col,
        opts->order_col
      );
      return result;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
}