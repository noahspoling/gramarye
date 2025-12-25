# DungeonDelve Test Suite

A modular test framework for the DungeonDelve project.

## Structure

- `test_runner.c` - Main test runner that orchestrates all tests
- `test_*.c` - Individual test modules (one per module/component)
- `test_common.h` - Common utilities and stubs for tests
- `build/` - Build output directory (created automatically)

## Usage

### Build and run all tests
```bash
./build_test.sh
```

### Run a specific test
```bash
./build_test.sh int_coord_hash
./build_test.sh table_operations
```

### List all available tests
```bash
./build_test.sh --list
```

### Show help
```bash
./build_test.sh --help
```

## Adding New Tests

To add a new test module:

1. Create a new file `tests/test_<module_name>.c`
2. Implement a test function with signature: `bool test_<module_name>(void)`
3. Register it in `test_runner.c`:
   ```c
   extern bool test_<module_name>(void);
   
   static TestCase test_registry[] = {
       // ... existing tests ...
       { "<module_name>", test_<module_name> },
       { NULL, NULL }
   };
   ```
4. Add the test file to `build_test.sh` compilation:
   ```bash
   "$TEST_DIR/test_<module_name>.c" \
   ```

## Test Module Template

```c
#include <stdio.h>
#include <stdbool.h>
#include "test_common.h"
// Include headers for module being tested

static bool test_feature_one(void) {
    printf("  Testing feature one...\n");
    // Your test code here
    if (/* test fails */) {
        printf("    ✗ FAILED: reason\n");
        return false;
    }
    printf("    ✓ Feature one test passed\n");
    return true;
}

static bool test_feature_two(void) {
    printf("  Testing feature two...\n");
    // Your test code here
    if (/* test fails */) {
        printf("    ✗ FAILED: reason\n");
        return false;
    }
    printf("    ✓ Feature two test passed\n");
    return true;
}

bool test_<module_name>(void) {
    bool all_passed = true;
    all_passed &= test_feature_one();
    all_passed &= test_feature_two();
    return all_passed;
}
```

## Current Test Modules

- `int_coord_hash` - Tests for IntCoord hash and comparison functions
- `table_operations` - Tests for Table operations with IntCoord keys

