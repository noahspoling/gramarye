#!/bin/bash
# Clean build script for gramarye project

# Check for flags
CLEAN=false
USE_LOCAL=false

for arg in "$@"; do
    case "$arg" in
        --clean|-c)
            CLEAN=true
            ;;
        --local|-l)
            USE_LOCAL=true
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --clean, -c    Perform a full clean build"
            echo "  --local, -l   Use local module folders instead of FetchContent"
            echo "  --help, -h     Show this help message"
            exit 0
            ;;
    esac
done

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

# Build CMake command with optional flags
CMAKE_ARGS=".."
if [ "$USE_LOCAL" == "true" ]; then
    echo "Using local modules..."
    CMAKE_ARGS="$CMAKE_ARGS -DUSE_LOCAL_MODULES=ON"
fi

cmake $CMAKE_ARGS
make
cd ..