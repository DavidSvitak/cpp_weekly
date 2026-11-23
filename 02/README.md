# Configuration file checker

## Objective

Create a command-line utility that validates and normalizes a simple `KEY=VALUE` configuration file.

The exercise practises:

- using `enum class` to represent parsing results;
- using `std::map` for sorted key-value data;
- separating parsing logic from command-line handling;
- file input, validation, diagnostics, and CMake testing.

## Program interface

The executable must be named `config_check`:

```bash
./build/config_check <input.conf>
```

Exactly one argument is required.

## Input format

Each physical line is one of the following:

- an empty line;
- a comment whose first non-whitespace character is `#`;
- a configuration assignment:

```text
KEY=VALUE
```

### Whitespace

Trim spaces, tabs, and carriage returns from:

- the beginning and end of the line;
- both sides of `=`.

Whitespace inside a value must be preserved.

### Valid keys

A key must:

- be nonempty;
- begin with an ASCII letter or `_`;
- contain only ASCII letters, digits, and `_`.

Keys are case-sensitive.

### Valid values

A value must be nonempty after trimming.

A valid assignment contains exactly one `=`. Quoting and escaping are not supported.

## Requirements

1. Require exactly one command-line argument.

2. If the argument count is invalid, print this to standard error:

   ```text
   Usage: config_check <input.conf>
   ```

3. If the file cannot be opened, print:

   ```text
   Error: cannot open '<path>'
   ```

   Replace `<path>` with the argument exactly as supplied.

4. Number physical input lines from `1`, including comments and empty lines.

5. Ignore comments and empty lines without producing a warning.

6. For every malformed assignment, print:

   ```text
   Warning: skipping malformed line N
   ```

7. When a valid key occurs more than once, the last value replaces the previous value. Print:

   ```text
   Warning: duplicate key 'KEY' on line N; replacing previous value
   ```

8. Print warnings to standard error in input order.

9. After reading the complete file, print the final assignments to standard output:

   ```text
   KEY=VALUE
   ```

10. Sort output lexicographically by key in ascending order.

11. Do not print a header.

12. Produce empty standard output when the file contains no valid assignments.

## Exit codes

| Exit code | Meaning |
|---:|---|
| `0` | The file was processed, even if malformed lines or duplicate keys produced warnings. |
| `1` | Invalid command-line arguments or the file could not be opened. |

For exit code `1`, standard output must be empty.

## Required public interface

Tests include `config.hpp` and expect these declarations:

```cpp
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

}
```

For `ignored` and `malformed`, `ParsedLine::key` and `value` are unspecified.

Strings stored in `LoadResult::warnings` must not contain trailing newline characters.

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

Run the program:

```bash
./build/config_check 02/test/data/normal.conf
```

## Tests

Run exercise `02`:

```bash
ctest --test-dir build -R "^02\\." --output-on-failure
```

Run its unit tests:

```bash
ctest --test-dir build -L unit -R "^02\\." --output-on-failure
```

Run its end-to-end tests:

```bash
ctest --test-dir build -L e2e -R "^02\\." --output-on-failure
```

Run only the parsing unit tests:

```bash
./build/02/test/config_unit_tests "[parse_line]"
```

## Test matrix

| Case | Arguments | Expected stdout | Expected stderr | Exit |
|---|---|---|---|---:|
| Normal file | `normal.conf` | `normal.expected` | empty | `0` |
| Whitespace and duplicate | `whitespace_and_duplicates.conf` | `whitespace_and_duplicates.expected` | `whitespace_and_duplicates.expected_err` | `0` |
| Malformed lines | `malformed.conf` | `malformed.expected` | `malformed.expected_err` | `0` |
| Empty file | `empty.conf` | empty | empty | `0` |
| No argument | none | empty | `usage.expected_err` | `1` |
| Too many arguments | `normal.conf extra.conf` | empty | `usage.expected_err` | `1` |
| Missing file | `does_not_exist.conf` | empty | `missing_file.expected_err` | `1` |

When stderr is expected to be empty, the corresponding `.expected_err` file is intentionally omitted.

## Hints

- A `std::map<std::string, std::string>` keeps keys sorted.
- Keep `parse_line()` independent of files and command-line arguments.
- `std::getline()` is useful for retaining accurate physical line numbers.

## Common pitfalls

- Printing warnings to `stdout` instead of `stderr`.
- Forgetting that comments and empty lines still affect line numbering.
- Rejecting spaces inside a value.
- Treating a malformed duplicate as a valid replacement.
- Forgetting that `std::getline()` may leave a trailing `\r` when reading CRLF data.