#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Test framework
typedef struct {
    const char *name;
    bool (*test_func)(void);
} TestCase;

// Forward declarations for test modules
extern bool test_int_coord_hash(void);
extern bool test_table_operations(void);
// Add more test modules here as they're created

// Test registry
static TestCase test_registry[] = {
    { "int_coord_hash", test_int_coord_hash },
    { "table_operations", test_table_operations },
    { NULL, NULL } // Sentinel
};

// Test statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Run a single test
static bool run_test(const char *name, bool (*test_func)(void)) {
    printf("\n=== Running test: %s ===\n", name);
    tests_run++;
    
    bool result = test_func();
    
    if (result) {
        printf("✓ PASSED: %s\n", name);
        tests_passed++;
        return true;
    } else {
        printf("✗ FAILED: %s\n", name);
        tests_failed++;
        return false;
    }
}

// Run all tests
static void run_all_tests(void) {
    printf("Running all tests...\n");
    printf("========================================\n");
    
    for (int i = 0; test_registry[i].name != NULL; i++) {
        run_test(test_registry[i].name, test_registry[i].test_func);
    }
}

// Run a specific test by name
static bool run_test_by_name(const char *name) {
    for (int i = 0; test_registry[i].name != NULL; i++) {
        if (strcmp(test_registry[i].name, name) == 0) {
            return run_test(name, test_registry[i].test_func);
        }
    }
    
    printf("Error: Test '%s' not found\n", name);
    printf("Available tests:\n");
    for (int i = 0; test_registry[i].name != NULL; i++) {
        printf("  - %s\n", test_registry[i].name);
    }
    return false;
}

// List all available tests
static void list_tests(void) {
    printf("Available tests:\n");
    for (int i = 0; test_registry[i].name != NULL; i++) {
        printf("  - %s\n", test_registry[i].name);
    }
}

// Print usage information
static void print_usage(const char *program_name) {
    printf("Usage: %s [options] [test_name]\n", program_name);
    printf("\nOptions:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -l, --list     List all available tests\n");
    printf("  -a, --all      Run all tests (default if no arguments)\n");
    printf("\nExamples:\n");
    printf("  %s                    # Run all tests\n", program_name);
    printf("  %s -a                 # Run all tests\n", program_name);
    printf("  %s int_coord_hash     # Run specific test\n", program_name);
    printf("  %s --list             # List all tests\n", program_name);
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("  DungeonDelve Test Suite\n");
    printf("========================================\n");
    
    // Parse command line arguments
    if (argc == 1) {
        // No arguments - run all tests
        run_all_tests();
    } else if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0) {
            list_tests();
            return 0;
        } else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--all") == 0) {
            run_all_tests();
        } else {
            // Assume it's a test name
            run_test_by_name(argv[1]);
        }
    } else {
        printf("Error: Too many arguments\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Print summary
    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Total:  %d\n", tests_run);
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}

