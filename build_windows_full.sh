#!/bin/bash

# Script to build the keylogger for Windows using cross-compilation
# Author: Keylogger Development Team
# Date: February 2026

echo "========================================="
echo "Keylogger Windows Build Script (Cross-compile)"
echo "========================================="

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Error: This script is intended for Linux systems only for cross-compilation."
    exit 1
fi

echo "Checking for required tools..."

# Check for MinGW-w64 cross-compiler
MINGW64_CC=""
MINGW32_CC=""

if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    MINGW64_CC="x86_64-w64-mingw32-gcc"
    echo "Found 64-bit MinGW-w64 cross-compiler: $MINGW64_CC"
fi

if command -v i686-w64-mingw32-gcc &> /dev/null; then
    MINGW32_CC="i686-w64-mingw32-gcc"
    echo "Found 32-bit MinGW-w64 cross-compiler: $MINGW32_CC"
fi

if [ -z "$MINGW64_CC" ] && [ -z "$MINGW32_CC" ]; then
    echo "Error: MinGW-w64 cross-compiler not found."
    echo ""
    echo "Install MinGW-w64 cross-compiler:"
    echo "Ubuntu/Debian: sudo apt-get install gcc-mingw-w64"
    echo "CentOS/RHEL/Fedora: sudo yum install mingw64-gcc mingw32-gcc"
    echo "Arch Linux: sudo pacman -S mingw-w64-gcc"
    echo ""
    echo "Alternatively, install on Windows with MSYS2:"
    echo "Download from: https://www.msys2.org/"
    echo "Then install mingw-w64 toolchain"
    exit 1
fi

echo ""
echo "Starting Windows build process..."
echo ""

# Build for 64-bit Windows if available
if [ -n "$MINGW64_CC" ]; then
    echo "Building 64-bit Windows executable..."
    $MINGW64_CC -o keylogger_x64.exe -D_WIN32 keylogger_cross_platform.c -luser32 -lkernel32
    
    if [ $? -eq 0 ]; then
        echo "64-bit Windows executable built successfully: keylogger_x64.exe"
        ls -la keylogger_x64.exe
    else
        echo "64-bit build failed!"
    fi
fi

# Build for 32-bit Windows if available
if [ -n "$MINGW32_CC" ]; then
    echo ""
    echo "Building 32-bit Windows executable..."
    $MINGW32_CC -o keylogger_x86.exe -D_WIN32 keylogger_cross_platform.c -luser32 -lkernel32
    
    if [ $? -eq 0 ]; then
        echo "32-bit Windows executable built successfully: keylogger_x86.exe"
        ls -la keylogger_x86.exe
    else
        echo "32-bit build failed!"
    fi
fi

echo ""
echo "========================================="
echo "Windows Build Process Complete"
echo "========================================="
echo "Note: Windows executables require administrator privileges to run"
echo "for global keyboard monitoring functionality."