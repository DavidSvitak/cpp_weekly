# C++ Weekly

[![CI](https://github.com/DavidSvitak/cpp_weekly/actions/workflows/ci.yml/badge.svg)](https://github.com/DavidSvitak/cpp_weekly/actions/workflows/ci.yml)

A collection of small, self-contained C++20 exercises, one per folder (`01/`, `02/`, ...).
Every exercise is built with CMake, covered by unit and end-to-end tests, and checked by CI
on every push and pull request.

## Exercises

| Folder | Program | Description |
|---|---|---|
| [`01/`](01/README.md) | `csv_summary` | Reads expenses from a CSV file and summarizes them by category. |
| [`02/`](02/README.md) | `config_check` | Validates and normalizes simple `KEY=VALUE` configuration files. |

Each folder has its own `README.md` with the assignment.

## Requirements

- A C++20 compiler: GCC 11+, Clang 16+ or MSVC 19.30+
- CMake 3.20+
- Git (CMake downloads [Catch2](https://github.com/catchorg/Catch2) for the tests at configure time)
- Optional: [Ninja](https://ninja-build.org/) for faster builds

## Build

From the repository root:

```bash
cmake -S . -B build
```

```bash
cmake --build build
```

Every program is placed directly in `build/`, for example:

```bash
./build/csv_summary 01/test/data/readme_example.csv
```

### Build options

| Option | Default | Effect |
|---|---|---|
| `BUILD_TESTING` | `ON` | Build the tests. `-DBUILD_TESTING=OFF` builds only the programs and skips downloading Catch2. |
| `CPP_WEEKLY_WARNINGS_AS_ERRORS` | `OFF` | Treat compiler warnings as errors. CI turns this on. |
| `CMAKE_BUILD_TYPE` | *(none)* | `Debug` or `Release` (single-config generators such as Make and Ninja). |

Example: 
```bash
cmake -S . -B build /
-DCMAKE_BUILD_TYPE=Debug / 
-DCPP_WEEKLY_WARNINGS_AS_ERRORS=ON
```

## Tests

Run every test of every exercise:

```bash
ctest --test-dir build --output-on-failure
```

There are two kinds of tests, each with its own CTest label:

- **`unit`**: [Catch2](https://github.com/catchorg/Catch2) tests in `NN/test/unit/` that call functions directly.
- **`e2e`**: end-to-end tests that run the real program and compare its stdout, stderr and exit code with expected files.

```bash
ctest --test-dir build -L unit
```

```bash
ctest --test-dir build -L e2e
```

Test names start with their folder, so one exercise can be run on its own:

```bash
ctest --test-dir build -R "^01\."
```

The Catch2 tests of an exercise are also a normal program, which accepts Catch2's own options (`--help` lists them):

```bash
./build/01/test/csv_unit_tests "[parse_amount]"
```

### End-to-end test data

End-to-end cases live in `NN/test/data/` and are picked up automatically:

| File | Meaning |
|---|---|
| `name.csv` | Input passed to the program |
| `name.expected` | Exact expected **stdout** |
| `name.expected_err` | Exact expected **stderr**. If this file is missing, stderr must be **empty**. |

To add a case, add a new `.csv` + `.expected` pair (and `.expected_err` if the program should print warnings).
No CMake change is needed. Cases that need extra arguments or a nonzero exit code are added by hand
with `add_cli_test()` (see [`cmake/CliTest.cmake`](cmake/CliTest.cmake)).

Test data is stored byte-for-byte (see [`.gitattributes`](.gitattributes)), so files such as
`windows_line_endings.csv` keep their exact line endings on every platform.

## Continuous integration

[`.github/workflows/ci.yml`](.github/workflows/ci.yml) builds and tests the whole repository on every
push and every pull request, in three configurations:

| Job | What it catches |
|---|---|
| GCC | Build errors, warnings (as errors) and test failures |
| Clang | The same with a second compiler |
| GCC + sanitizers | Memory errors (AddressSanitizer) and undefined behaviour (UndefinedBehaviorSanitizer) |

A run can also be started by hand from the **Actions** tab (**Run workflow**).

## Repository layout

```
cpp_weekly/
├─ CMakeLists.txt            shared setup: C++20, warnings, Catch2, add_subdirectory() per exercise
├─ cmake/
│  ├─ CliTest.cmake          add_cli_test(): registers an end-to-end test
│  └─ run_cli_test.cmake     runs a program and checks stdout, stderr and exit code
├─ .github/workflows/ci.yml  CI on push and pull request
└─ 01/
   ├─ README.md              the assignment
   ├─ CMakeLists.txt         library + program + tests for this exercise
   ├─ include/  src/         the code
   └─ test/
      ├─ CMakeLists.txt      registers this exercise's tests
      ├─ unit/               Catch2 unit tests
      └─ data/               end-to-end inputs and expected outputs
```

## Adding a new exercise

1. Create the folder (e.g. `02/`) with `README.md`, `include/`, `src/` and `test/`.
2. In `02/CMakeLists.txt`, put everything except `main.cpp` in a library, so the program and the tests share
   the same code. Link `cpp_weekly_warnings` to every target to get the shared compiler warnings:

   ```cmake
   add_library(my_lib STATIC src/...)
   target_include_directories(my_lib PUBLIC include)
   target_compile_features(my_lib PUBLIC cxx_std_20)
   target_link_libraries(my_lib PRIVATE cpp_weekly_warnings)

   add_executable(my_program src/main.cpp)
   target_link_libraries(my_program PRIVATE my_lib cpp_weekly_warnings)
   set_target_properties(my_program PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

   if(BUILD_TESTING)
       add_subdirectory(test)
   endif()
   ```

3. In `02/test/CMakeLists.txt`, register the tests with the `02.` prefix:

   ```cmake
   add_executable(my_unit_tests unit/test_something.cpp)
   target_link_libraries(my_unit_tests PRIVATE my_lib Catch2::Catch2WithMain cpp_weekly_warnings)
   catch_discover_tests(my_unit_tests TEST_PREFIX "02." PROPERTIES LABELS "unit")
   ```

4. Add `add_subdirectory(02)` to the root `CMakeLists.txt`.


## Workflow

- Each exercise is developed on its own branch (for example `01-csv_summary`) and merged into `main`
  through a pull request.
- CI must pass before merging (enable **Require status checks to pass** for `main` in the repository's
  branch protection settings).
- Build directories (`build/`, `build-*/`) are ignored by git.
