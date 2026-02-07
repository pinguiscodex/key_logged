#!/bin/bash

# Script to build the keylogger for Linux
# Author: Keylogger Development Team
# Date: February 2026

echo "========================================="
echo "Keylogger Linux Build Script"
echo "========================================="

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Error: This script is intended for Linux systems only."
    exit 1
fi

echo "Checking for required tools..."

# Check for GCC
if ! command -v gcc &> /dev/null; then
    echo "Error: GCC compiler not found. Please install build-essential or gcc."
    echo "Ubuntu/Debian: sudo apt-get install build-essential"
    echo "CentOS/RHEL/Fedora: sudo yum install gcc make"
    echo "Arch Linux: sudo pacman -S gcc make"
    exit 1
fi

echo "GCC found: $(gcc --version | head -n1)"

# Check for make
if ! command -v make &> /dev/null; then
    echo "Error: make not found. Please install make."
    exit 1
fi

echo "Make found: $(make --version | head -n1)"

# Check for required libraries
echo "Checking for required libraries..."
if ! ldconfig -p | grep -q librt; then
    echo "Warning: librt might not be available, but it's usually part of glibc"
fi

echo "Starting build process..."
echo ""

# Clean previous builds
echo "Cleaning previous builds..."
make clean

# Build the Linux executable
echo "Building Linux executable..."
make

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "Build successful!"
    echo "Linux executable: keylogger"
    echo "========================================="
    
    # Show file info
    if [ -f "./keylogger" ]; then
        echo "File information:"
        ls -la ./keylogger
        echo ""
        echo "To run: sudo ./keylogger"
        echo "Note: Requires root privileges to access input devices"
    fi
else
    echo "Build failed!"
    exit 1
fi