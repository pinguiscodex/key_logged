#!/bin/bash

# Unified build script for keylogger - Linux and Windows
# Author: Keylogger Development Team
# Date: February 2026

echo "========================================="
echo "Keylogger Universal Build Script"
echo "Supports: Linux and Windows (cross-compile)"
echo "========================================="

BUILD_LINUX=false
BUILD_WINDOWS=false
CLEAN_ONLY=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -l|--linux)
            BUILD_LINUX=true
            shift
            ;;
        -w|--windows)
            BUILD_WINDOWS=true
            shift
            ;;
        -a|--all)
            BUILD_LINUX=true
            BUILD_WINDOWS=true
            shift
            ;;
        -c|--clean)
            CLEAN_ONLY=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  -l, --linux    Build for Linux"
            echo "  -w, --windows  Build for Windows (cross-compile)"
            echo "  -a, --all      Build for all platforms"
            echo "  -c, --clean    Clean build artifacts only"
            echo "  -h, --help     Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 --linux                     # Build for Linux only"
            echo "  $0 --windows                   # Build for Windows only"
            echo "  $0 --all                       # Build for all platforms"
            echo "  $0 --clean                     # Clean build artifacts"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# If no specific platform was requested, default to current platform
if [ "$BUILD_LINUX" = false ] && [ "$BUILD_WINDOWS" = false ] && [ "$CLEAN_ONLY" = false ]; then
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        BUILD_LINUX=true
        echo "Auto-detected Linux platform, building for Linux"
    else
        echo "Unsupported platform for auto-build. Please specify platform explicitly."
        exit 1
    fi
fi

# Clean function
clean_build() {
    echo "Cleaning build artifacts..."
    make clean
    rm -f keylogger_x64.exe keylogger_x86.exe
    echo "Clean complete."
}

# Build for Linux
build_linux() {
    if [[ "$OSTYPE" != "linux-gnu"* ]]; then
        echo "Error: Linux build requested but not on Linux platform."
        return 1
    fi
    
    echo ""
    echo "Building for Linux..."
    echo "===================="
    
    # Check for required tools
    if ! command -v gcc &> /dev/null; then
        echo "Error: GCC compiler not found."
        return 1
    fi
    
    if ! command -v make &> /dev/null; then
        echo "Error: make not found."
        return 1
    fi
    
    # Build
    make
    if [ $? -eq 0 ]; then
        echo ""
        echo "✓ Linux build completed successfully!"
        echo "Executable: keylogger"
        ls -la keylogger 2>/dev/null || echo "File not found"
    else
        echo "✗ Linux build failed!"
        return 1
    fi
}

# Build for Windows
build_windows() {
    if [[ "$OSTYPE" != "linux-gnu"* ]]; then
        echo "Error: Windows cross-compilation requires Linux platform."
        return 1
    fi
    
    echo ""
    echo "Building for Windows (Cross-compile)..."
    echo "====================================="
    
    # Check for MinGW-w64 cross-compiler
    MINGW64_CC=""
    MINGW32_CC=""
    
    if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
        MINGW64_CC="x86_64-w64-mingw32-gcc"
        echo "✓ Found 64-bit MinGW-w64 cross-compiler"
    fi
    
    if command -v i686-w64-mingw32-gcc &> /dev/null; then
        MINGW32_CC="i686-w64-mingw32-gcc"
        echo "✓ Found 32-bit MinGW-w64 cross-compiler"
    fi
    
    if [ -z "$MINGW64_CC" ] && [ -z "$MINGW32_CC" ]; then
        echo "✗ Error: MinGW-w64 cross-compiler not found."
        echo "Install with: sudo apt-get install gcc-mingw-w64"
        return 1
    fi
    
    # Build for 64-bit Windows if available
    if [ -n "$MINGW64_CC" ]; then
        echo "Building 64-bit Windows executable..."
        $MINGW64_CC -o keylogger_x64.exe -D_WIN32 keylogger_cross_platform.c -luser32 -lkernel32
        
        if [ $? -eq 0 ]; then
            echo "✓ 64-bit Windows executable built: keylogger_x64.exe"
            ls -la keylogger_x64.exe
        else
            echo "✗ 64-bit Windows build failed!"
        fi
    fi
    
    # Build for 32-bit Windows if available
    if [ -n "$MINGW32_CC" ]; then
        echo ""
        echo "Building 32-bit Windows executable..."
        $MINGW32_CC -o keylogger_x86.exe -D_WIN32 keylogger_cross_platform.c -luser32 -lkernel32
        
        if [ $? -eq 0 ]; then
            echo "✓ 32-bit Windows executable built: keylogger_x86.exe"
            ls -la keylogger_x86.exe
        else
            echo "✗ 32-bit Windows build failed!"
        fi
    fi
    
    return 0
}

# Execute requested actions
if [ "$CLEAN_ONLY" = true ]; then
    clean_build
    exit 0
fi

if [ "$BUILD_LINUX" = true ]; then
    build_linux
    LINUX_RESULT=$?
else
    LINUX_RESULT=0  # Not building, so success
fi

if [ "$BUILD_WINDOWS" = true ]; then
    build_windows
    WINDOWS_RESULT=$?
else
    WINDOWS_RESULT=0  # Not building, so success
fi

# Final status
echo ""
echo "========================================="
echo "Build Summary"
echo "========================================="
if [ "$BUILD_LINUX" = true ]; then
    if [ $LINUX_RESULT -eq 0 ]; then
        echo "Linux: ✓ Success"
    else
        echo "Linux: ✗ Failed"
    fi
fi

if [ "$BUILD_WINDOWS" = true ]; then
    if [ $WINDOWS_RESULT -eq 0 ]; then
        echo "Windows: ✓ Success"
    else
        echo "Windows: ✗ Failed"
    fi
fi

if [ $LINUX_RESULT -eq 0 ] && [ $WINDOWS_RESULT -eq 0 ]; then
    echo ""
    echo "🎉 All requested builds completed successfully!"
    exit 0
else
    echo ""
    echo "❌ Some builds failed. Check the output above for details."
    exit 1
fi