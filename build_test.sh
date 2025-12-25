#!/bin/bash

# Build script for the test suite
# Usage: ./build_test.sh [test_name]
#   - No arguments: builds and runs all tests
#   - test_name: builds and runs specific test

set -e  # Exit on error

TEST_DIR="tests"
BUILD_DIR="$TEST_DIR/build"
BINARY_NAME="test_runner"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Create a temporary table.c with TraceLog stubbed out
TEMP_TABLE="/tmp/table_test_$$.c"
sed 's/#include "raylib.h"/\/\/ #include "raylib.h"\n#define LOG_INFO 0\n#define LOG_DEBUG 1\n#define LOG_WARNING 2\n#define LOG_ERROR 3\n#define TraceLog(level, ...) ((void)0)/' src/core/table.c > "$TEMP_TABLE"

# Cleanup function
cleanup() {
    rm -f "$TEMP_TABLE"
}
trap cleanup EXIT

echo "Building test suite..."

# Build the test runner with all test modules
gcc -std=c99 \
    -I./include \
    -I./include/core \
    -I./include/core/hash \
    -I./include/components \
    -I./tests \
    "$TEST_DIR/test_runner.c" \
    "$TEST_DIR/test_int_coord_hash.c" \
    "$TEST_DIR/test_table_operations.c" \
    src/core/hash/int_coord_hash.c \
    "$TEMP_TABLE" \
    src/core/arena.c \
    src/core/mem.c \
    src/core/except.c \
    src/core/assert.c \
    -o "$BUILD_DIR/$BINARY_NAME" \
    -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    
    # Run tests
    if [ $# -eq 0 ]; then
        # No arguments - run all tests
        "$BUILD_DIR/$BINARY_NAME"
    else
        # Pass arguments to test runner
        "$BUILD_DIR/$BINARY_NAME" "$@"
    fi
else
    echo "Build failed!"
    exit 1
fi
