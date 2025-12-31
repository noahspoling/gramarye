#!/bin/bash
# Clean build script for gramarye project

# Check for clean flag
CLEAN=false
if [ "$1" == "--clean" ] || [ "$1" == "-c" ]; then
    CLEAN=true
fi

# Remove old CMake cache and FetchContent dependencies (fixes cache mismatch errors)
if [ "$CLEAN" == "true" ] && [ -d "bin" ]; then
    echo "Performing full clean..."
    rm -rf bin
elif [ -d "bin" ]; then
    echo "Cleaning CMake cache files..."
    rm -rf bin/CMakeCache.txt
    rm -rf bin/CMakeFiles
    # Clean FetchContent caches in _deps subdirectories
    if [ -d "bin/_deps" ]; then
        find bin/_deps -name "CMakeCache.txt" -delete 2>/dev/null || true
        find bin/_deps -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
    fi
fi

mkdir -p bin
cd bin
cmake ..
make
cd ..