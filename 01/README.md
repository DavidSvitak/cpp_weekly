# CSV expense summary

## Objective

Create a command-line utility that reads expenses from a CSV file and summarizes them by category.

The exercise practises:
- command-line arguments;
- file and CSV input;
- STL containers;
- sorting;
- basic input validation;
- CMake builds.

## Program interface 

Your executable must be named `csv_summary` and invoked as:  
```bash
./build/csv_summary <input.csv>
```

## Input format

The CSV begins with a header:  
```csv
date,category,amount
```  
Every subsequent valid row contains exactly three fields:  
```csv
date,category,amount
```  
You may assume that fields do not contain quoted commas.

## Requirements

1. Require exactly one command-line argument.

2. Open the specified CSV file and skip its header.

3. For each category, calculate:
    - number of expenses;
    - total amount.

4. Sort the results:
    - primarily by total, from largest to smallest;
    - alphabetically by category when totals are equal.

5. Print monetary values with exactly two decimal places.

6. Skip malformed rows and print this warning to standard error:
    ```text
    Warning: skipping malformed line N
    ```
7. A row is malformed when:
    - it does not contain exactly three fields;
    - its category is empty;
    - its amount is not entirely numeric.

8. Return a nonzero exit code if:
    - the argument is missing;
    - too many arguments are provided;
    - the file cannot be opened.

9. Provide a CMakeLists.txt that enables C++20 and builds the executable.






