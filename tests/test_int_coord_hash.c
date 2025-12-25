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

// Test the hash function
static bool test_hash_consistency(void) {
    printf("  Testing hash consistency...\n");
    
    IntCoord c1 = {0, 0};
    IntCoord c2 = {58, 91};
    IntCoord c3 = {4, 6};
    IntCoord c4 = {58, 91}; // Same as c2
    
    unsigned h1 = IntCoord_hash(&c1);
    unsigned h2 = IntCoord_hash(&c2);
    unsigned h3 = IntCoord_hash(&c3);
    unsigned h4 = IntCoord_hash(&c4);
    
    printf("    Hash(0, 0) = %u\n", h1);
    printf("    Hash(58, 91) = %u\n", h2);
    printf("    Hash(4, 6) = %u\n", h3);
    printf("    Hash(58, 91) again = %u\n", h4);
    
    if (h2 != h4) {
        printf("    ✗ FAILED: Same coordinates produced different hashes\n");
        return false;
    }
    
    printf("    ✓ Hash consistency test passed\n");
    return true;
}

// Test the comparison function
static bool test_comparison_function(void) {
    printf("  Testing comparison function...\n");
    
    IntCoord c1 = {0, 0};
    IntCoord c2 = {58, 91};
    IntCoord c3 = {4, 6};
    IntCoord c4 = {58, 91}; // Same as c2
    IntCoord c5 = {58, 92}; // Different y
    
    int cmp1 = IntCoord_cmp(&c1, &c1);
    int cmp2 = IntCoord_cmp(&c2, &c4);
    int cmp3 = IntCoord_cmp(&c2, &c3);
    int cmp4 = IntCoord_cmp(&c2, &c5);
    
    printf("    Cmp((0,0), (0,0)) = %d (should be 0)\n", cmp1);
    printf("    Cmp((58,91), (58,91)) = %d (should be 0)\n", cmp2);
    printf("    Cmp((58,91), (4,6)) = %d (should be != 0)\n", cmp3);
    printf("    Cmp((58,91), (58,92)) = %d (should be != 0)\n", cmp4);
    
    if (cmp1 != 0) {
        printf("    ✗ FAILED: Same coordinates should compare equal\n");
        return false;
    }
    if (cmp2 != 0) {
        printf("    ✗ FAILED: Same coordinates should compare equal\n");
        return false;
    }
    if (cmp3 == 0) {
        printf("    ✗ FAILED: Different coordinates should compare unequal\n");
        return false;
    }
    if (cmp4 == 0) {
        printf("    ✗ FAILED: Different coordinates should compare unequal\n");
        return false;
    }
    
    printf("    ✓ Comparison function test passed\n");
    return true;
}

// Test hash collision detection
static bool test_hash_collisions(void) {
    printf("  Testing hash collisions...\n");
    
    IntCoord test_coords[] = {
        {0, 0}, {58, 91}, {4, 6}, {1, 1}, {2, 2}, {3, 3},
        {10, 10}, {20, 20}, {30, 30}, {100, 100}
    };
    int num_coords = sizeof(test_coords) / sizeof(test_coords[0]);
    
    unsigned hashes[10];
    for (int i = 0; i < num_coords; i++) {
        hashes[i] = IntCoord_hash(&test_coords[i]);
        printf("    Hash(%d, %d) = %u\n", test_coords[i].x, test_coords[i].y, hashes[i]);
    }
    
    // Check for collisions (same hash with different coordinates)
    int collision_count = 0;
    for (int i = 0; i < num_coords; i++) {
        for (int j = i + 1; j < num_coords; j++) {
            if (hashes[i] == hashes[j]) {
                printf("    ⚠ WARNING: Hash collision between (%d, %d) and (%d, %d) = %u\n",
                       test_coords[i].x, test_coords[i].y,
                       test_coords[j].x, test_coords[j].y,
                       hashes[i]);
                collision_count++;
            }
        }
    }
    
    if (collision_count > 0) {
        printf("    ⚠ Found %d hash collision(s) (this is OK, table handles collisions)\n", collision_count);
    } else {
        printf("    ✓ No hash collisions in test set\n");
    }
    
    return true; // Collisions are OK, table handles them
}

// Main test function for int_coord_hash module
bool test_int_coord_hash(void) {
    bool all_passed = true;
    
    all_passed &= test_hash_consistency();
    all_passed &= test_comparison_function();
    all_passed &= test_hash_collisions();
    
    return all_passed;
}
