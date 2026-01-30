#!/bin/bash
# Build script for Linux/macOS
# Usage: ./build.sh or ./build.sh clean

set -e

if [ "$1" = "clean" ]; then
    echo "Cleaning build artifacts..."
    rm -f mylib.so mylib.dylib mylib.o
    echo "Build cleaned successfully"
    exit 0
fi

echo "Detecting platform..."
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    LIB_NAME="mylib.so"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    LIB_NAME="mylib.dylib"
else
    echo "Unsupported platform: $OSTYPE"
    exit 1
fi

echo "Compiling mylib.c to $LIB_NAME..."
gcc -fPIC -shared -o "$LIB_NAME" mylib.c -Wall -Wextra -O2

echo ""
echo "============================================"
echo "Build successful: $LIB_NAME created"
echo "============================================"
echo ""
echo "Run the examples:"
echo "  python mylib_wrapper.py"
echo "  python simple_example.py"
echo ""
