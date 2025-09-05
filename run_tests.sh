#!/bin/bash

echo "======================================="
echo "      CodeMap Test Suite (Linux/WSL)"
echo "======================================="
echo

# Set build directory
BUILD_DIR="build"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR" || exit 1

# Configure with CMake if not already done
if [ ! -f "CMakeCache.txt" ]; then
    echo "Configuring project with CMake..."
    cmake ..
    if [ $? -ne 0 ]; then
        echo "CMake configuration failed!"
        cd ..
        exit 1
    fi
fi

# Build the tests
echo
echo "Building tests..."
cmake --build . --config Debug
if [ $? -ne 0 ]; then
    echo "Build failed!"
    cd ..
    exit 1
fi

echo
echo "======================================="
echo "        Running Tests"
echo "======================================="
echo

TEST_FAILED=0

# Run test_codemap_types
echo "Running test_codemap_types..."
echo "-------------------------------"
if [ -f "tests/Debug/test_codemap_types.exe" ]; then
    # Windows build output
    ./tests/Debug/test_codemap_types.exe
elif [ -f "tests/test_codemap_types" ]; then
    # Linux build output
    ./tests/test_codemap_types
elif [ -f "test_codemap_types" ]; then
    # Alternative Linux build output
    ./test_codemap_types
else
    echo "Error: test_codemap_types executable not found!"
    TEST_FAILED=1
fi

if [ $? -ne 0 ]; then
    echo "test_codemap_types FAILED!"
    TEST_FAILED=1
else
    echo "test_codemap_types PASSED!"
fi

echo

# Run test_parser
echo "Running test_parser..."
echo "-------------------------------"
if [ -f "tests/Debug/test_parser.exe" ]; then
    # Windows build output
    ./tests/Debug/test_parser.exe
elif [ -f "tests/test_parser" ]; then
    # Linux build output
    ./tests/test_parser
elif [ -f "test_parser" ]; then
    # Alternative Linux build output
    ./test_parser
else
    echo "Error: test_parser executable not found!"
    TEST_FAILED=1
fi

if [ $? -ne 0 ]; then
    echo "test_parser FAILED!"
    TEST_FAILED=1
else
    echo "test_parser PASSED!"
fi

echo

# Run test_json_exporter
echo "Running test_json_exporter..."
echo "-------------------------------"
if [ -f "tests/Debug/test_json_exporter.exe" ]; then
    # Windows build output
    ./tests/Debug/test_json_exporter.exe
elif [ -f "tests/test_json_exporter" ]; then
    # Linux build output
    ./tests/test_json_exporter
elif [ -f "test_json_exporter" ]; then
    # Alternative Linux build output
    ./test_json_exporter
else
    echo "Error: test_json_exporter executable not found!"
    TEST_FAILED=1
fi

if [ $? -ne 0 ]; then
    echo "test_json_exporter FAILED!"
    TEST_FAILED=1
else
    echo "test_json_exporter PASSED!"
fi

echo

# Run test_graph_builder
echo "Running test_graph_builder..."
echo "-------------------------------"
if [ -f "tests/Debug/test_graph_builder.exe" ]; then
    # Windows build output
    ./tests/Debug/test_graph_builder.exe
elif [ -f "tests/test_graph_builder" ]; then
    # Linux build output
    ./tests/test_graph_builder
elif [ -f "test_graph_builder" ]; then
    # Alternative Linux build output
    ./test_graph_builder
else
    echo "Error: test_graph_builder executable not found!"
    TEST_FAILED=1
fi

if [ $? -ne 0 ]; then
    echo "test_graph_builder FAILED!"
    TEST_FAILED=1
else
    echo "test_graph_builder PASSED!"
fi

echo
echo "======================================="

# Check if any test failed
if [ $TEST_FAILED -ne 0 ]; then
    echo "        SOME TESTS FAILED!"
    echo "======================================="
    cd ..
    exit 1
else
    echo "        ALL TESTS PASSED!"
    echo "======================================="
    cd ..
    exit 0
fi