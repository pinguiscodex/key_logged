#!/bin/bash

echo "Building Windows executable..."

# Check if mingw-w64 is installed
if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "Using x86_64-w64-mingw32-gcc to build Windows executable..."
    x86_64-w64-mingw32-gcc -o keylogger.exe -D_WIN32 keylogger_cross_platform.c -luser32
elif command -v i686-w64-mingw32-gcc &> /dev/null; then
    echo "Using i686-w64-mingw32-gcc to build Windows executable..."
    i686-w64-mingw32-gcc -o keylogger.exe -D_WIN32 keylogger_cross_platform.c -luser32
else
    echo "Error: MinGW-w64 cross-compiler not found."
    echo "Install MinGW-w64 with: sudo apt-get install gcc-mingw-w64"
    echo "Or on other systems: sudo yum install mingw64-gcc mingw32-gcc"
    echo "Or: sudo pacman -S mingw-w64-gcc"
    exit 1
fi

echo "Windows executable built successfully!"