#!/bin/bash
# Quick Test Runner Script
# Usage: ./run_tests.sh from the project root

echo "╔════════════════════════════════════════════════════╗"
echo "║     IntelliEditor - Gap Buffer Test Runner         ║"
echo "╚════════════════════════════════════════════════════╝"
echo ""

# Navigate to project root if not already there
PROJECT_ROOT="/mnt/space-2/COURS/LANGAGE C/intelliEditor"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ ERROR: CMakeLists.txt not found!"
    echo "Please run this script from the project root: $PROJECT_ROOT"
    exit 1
fi

echo "📁 Project Root: $(pwd)"
echo ""

# Step 1: Check if build directory exists
if [ ! -d "build" ]; then
    echo "📦 Creating build directory..."
    mkdir -p build
fi

# Step 2: Configure with CMake
echo "🔨 Configuring project with CMake..."
cd build
cmake .. > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    cmake ..
    exit 1
fi
echo "✓ CMake configuration successful"
echo ""

# Step 3: Build
echo "🔨 Building project..."
make > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    make
    exit 1
fi
echo "✓ Build successful"
echo ""

# Step 4: Run tests
echo "🧪 Running Gap Buffer Tests..."
echo "════════════════════════════════════════════════════"
echo ""

./test_gap_buffer

TEST_RESULT=$?

echo ""
echo "════════════════════════════════════════════════════"

if [ $TEST_RESULT -eq 0 ]; then
    echo "✓ All tests passed!"
    echo ""
    echo "📊 Next steps:"
    echo "   1. Review test results above"
    echo "   2. Commit your changes: git add . && git commit -m 'Add gap_buffer tests'"
    echo "   3. Push to repository: git push"
    exit 0
else
    echo "❌ Some tests failed!"
    echo ""
    echo "📊 Debugging tips:"
    echo "   1. Read the test output above carefully"
    echo "   2. Check gap_buffer.c implementation"
    echo "   3. Verify gap_buffer_* functions are complete"
    echo "   4. See README_TESTS.md for debugging guide"
    exit 1
fi
