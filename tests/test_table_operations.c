#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// Define TraceLog stub before including anything that uses it
#define LOG_INFO 0
#define LOG_DEBUG 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define TraceLog(level, ...) ((void)0)

#include "core/hash/int_coord_hash.h"
#include "core/table.h"
#include "core/arena.h"

// Test basic table put/get operations
static bool test_basic_table_operations(void) {
    printf("  Testing basic table operations...\n");
    
    Arena_T arena = Arena_new();
    Table_T table = Table_new(1024, IntCoord_cmp, IntCoord_hash);
    
    // Test setting and getting values
    IntCoord coord1 = {58, 91};
    IntCoord coord2 = {4, 6};
    IntCoord coord3 = {0, 0};
    
    int value1 = 100;
    int value2 = 200;
    int value3 = 300;
    
    // Put values
    Table_put(table, &coord1, &value1);
    Table_put(table, &coord2, &value2);
    Table_put(table, &coord3, &value3);
    
    printf("    Put value at (58, 91) = %d\n", value1);
    printf("    Put value at (4, 6) = %d\n", value2);
    printf("    Put value at (0, 0) = %d\n", value3);
    
    // Get values back with same IntCoord instances
    int *retrieved1 = (int*)Table_get(table, &coord1);
    int *retrieved2 = (int*)Table_get(table, &coord2);
    int *retrieved3 = (int*)Table_get(table, &coord3);
    
    printf("    Got value at (58, 91) = %d\n", retrieved1 ? *retrieved1 : -1);
    printf("    Got value at (4, 6) = %d\n", retrieved2 ? *retrieved2 : -1);
    printf("    Got value at (0, 0) = %d\n", retrieved3 ? *retrieved3 : -1);
    
    if (!retrieved1 || *retrieved1 != 100) {
        printf("    ✗ FAILED: Should retrieve value 100 for (58, 91)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    if (!retrieved2 || *retrieved2 != 200) {
        printf("    ✗ FAILED: Should retrieve value 200 for (4, 6)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    if (!retrieved3 || *retrieved3 != 300) {
        printf("    ✗ FAILED: Should retrieve value 300 for (0, 0)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    
    printf("    ✓ Basic table operations passed\n");
    
    Table_free(&table);
    Arena_dispose(&arena);
    return true;
}

// Test table operations with different IntCoord instances (like in Tilemap_get_tile)
static bool test_table_with_different_instances(void) {
    printf("  Testing table with different IntCoord instances...\n");
    
    Arena_T arena = Arena_new();
    Table_T table = Table_new(1024, IntCoord_cmp, IntCoord_hash);
    
    // Put values using one set of coordinates
    IntCoord coord1 = {58, 91};
    IntCoord coord2 = {4, 6};
    IntCoord coord3 = {0, 0};
    
    int value1 = 100;
    int value2 = 200;
    int value3 = 300;
    
    Table_put(table, &coord1, &value1);
    Table_put(table, &coord2, &value2);
    Table_put(table, &coord3, &value3);
    
    // Get values using different IntCoord instances (like Tilemap_get_tile does)
    IntCoord lookup1 = {58, 91};
    IntCoord lookup2 = {4, 6};
    IntCoord lookup3 = {0, 0};
    IntCoord lookup4 = {99, 99}; // Should not exist
    
    int *found1 = (int*)Table_get(table, &lookup1);
    int *found2 = (int*)Table_get(table, &lookup2);
    int *found3 = (int*)Table_get(table, &lookup3);
    int *found4 = (int*)Table_get(table, &lookup4);
    
    printf("    Lookup (58, 91) with new IntCoord = %d\n", found1 ? *found1 : -1);
    printf("    Lookup (4, 6) with new IntCoord = %d\n", found2 ? *found2 : -1);
    printf("    Lookup (0, 0) with new IntCoord = %d\n", found3 ? *found3 : -1);
    printf("    Lookup (99, 99) (should be NULL) = %p\n", (void*)found4);
    
    if (!found1 || *found1 != 100) {
        printf("    ✗ FAILED: Should retrieve value 100 for (58, 91)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    if (!found2 || *found2 != 200) {
        printf("    ✗ FAILED: Should retrieve value 200 for (4, 6)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    if (!found3 || *found3 != 300) {
        printf("    ✗ FAILED: Should retrieve value 300 for (0, 0)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    if (found4 != NULL) {
        printf("    ✗ FAILED: Should not find value for (99, 99)\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    
    printf("    ✓ Table operations with different instances passed\n");
    
    Table_free(&table);
    Arena_dispose(&arena);
    return true;
}

// Test table update operations
static bool test_table_updates(void) {
    printf("  Testing table update operations...\n");
    
    Arena_T arena = Arena_new();
    Table_T table = Table_new(1024, IntCoord_cmp, IntCoord_hash);
    
    IntCoord coord = {10, 20};
    int value1 = 100;
    int value2 = 200;
    
    // Put initial value
    void *prev = Table_put(table, &coord, &value1);
    if (prev != NULL) {
        printf("    ✗ FAILED: First put should return NULL\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    
    // Update with new value
    prev = Table_put(table, &coord, &value2);
    if (prev != &value1) {
        printf("    ✗ FAILED: Update should return previous value\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    
    // Verify new value is stored
    int *retrieved = (int*)Table_get(table, &coord);
    if (!retrieved || *retrieved != 200) {
        printf("    ✗ FAILED: Should retrieve updated value 200\n");
        Table_free(&table);
        Arena_dispose(&arena);
        return false;
    }
    
    printf("    ✓ Table update operations passed\n");
    
    Table_free(&table);
    Arena_dispose(&arena);
    return true;
}

// Main test function for table operations module
bool test_table_operations(void) {
    bool all_passed = true;
    
    all_passed &= test_basic_table_operations();
    all_passed &= test_table_with_different_instances();
    all_passed &= test_table_updates();
    
    return all_passed;
}

