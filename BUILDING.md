# Building Keylogger for All Platforms

This document explains how to build the keylogger for both Linux and Windows platforms.

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Building for Linux](#building-for-linux)
4. [Building for Windows](#building-for-windows)
5. [Unified Build Script](#unified-build-script)
6. [Troubleshooting](#troubleshooting)

## Overview

The keylogger supports cross-platform compilation:
- **Linux**: Native compilation using GCC
- **Windows**: Cross-compilation using MinGW-w64 from Linux

## Prerequisites

### For Linux Builds
- GCC compiler
- Make utility
- Linux kernel with input device support
- Root privileges to access input devices

### For Windows Cross-Compilation
- Linux system (Ubuntu, Debian, CentOS, etc.)
- MinGW-w64 cross-compiler
- Required libraries for Windows API

## Building for Linux

### Quick Build
```bash
# Make the script executable
chmod +x build_linux.sh

# Run the build script
./build_linux.sh
```

### Manual Build
```bash
# Clean previous builds
make clean

# Build the executable
make

# The resulting executable will be named 'keylogger'
```

### Running on Linux
```bash
# The keylogger needs root privileges to access input devices
sudo ./keylogger
```

## Building for Windows

### Installing MinGW-w64

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install gcc-mingw-w64
```

#### CentOS/RHEL/Fedora
```bash
sudo yum install mingw64-gcc mingw32-gcc
# or for newer versions:
sudo dnf install mingw64-gcc mingw32-gcc
```

#### Arch Linux
```bash
sudo pacman -S mingw-w64-gcc
```

### Cross-Compiling for Windows
```bash
# Make the script executable
chmod +x build_windows_full.sh

# Run the build script
./build_windows_full.sh
```

This will create:
- `keylogger_x64.exe` - 64-bit Windows executable
- `keylogger_x86.exe` - 32-bit Windows executable (if 32-bit compiler is available)

### Running on Windows
- Run as Administrator for global keyboard monitoring
- Place the executable in a convenient location
- The executable will create log files in the same directory

## Unified Build Script

A unified build script is available to build for multiple platforms:

### Make it executable
```bash
chmod +x build_all.sh
```

### Usage Options
```bash
# Show help
./build_all.sh --help

# Build for Linux only
./build_all.sh --linux

# Build for Windows only
./build_all.sh --windows

# Build for all platforms
./build_all.sh --all

# Clean build artifacts
./build_all.sh --clean
```

### Examples
```bash
# Build for all platforms
./build_all.sh --all

# Build only for Linux
./build_all.sh -l

# Build only for Windows
./build_all.sh -w
```

## Generated Executables

After building, you'll have the following executables:

| Platform | Executable Name | Architecture | Notes |
|----------|----------------|--------------|-------|
| Linux | `keylogger` | 64-bit | Requires root privileges |
| Windows | `keylogger_x64.exe` | 64-bit | Requires admin privileges |
| Windows | `keylogger_x86.exe` | 32-bit | Requires admin privileges |

## Configuration

Both executables use the same configuration file: `keylogger.conf`

Example configuration:
```
# Log Directory - Specify where log files should be stored
# Leave empty or use '.' to store in the same directory as the executable
# Example: /var/log/keylogger/ or /home/user/logs/
log_directory=
```

## Troubleshooting

### Common Linux Build Issues

**Error: gcc: command not found**
- Install build-essential: `sudo apt-get install build-essential`

**Error: Permission denied accessing /dev/input/eventX**
- Run with sudo: `sudo ./keylogger`

### Common Windows Cross-Compilation Issues

**Error: x86_64-w64-mingw32-gcc: command not found**
- Install MinGW-w64: `sudo apt-get install gcc-mingw-w64`

**Error: undefined reference to WinMain**
- Make sure to include `-luser32` and `-lkernel32` in the compilation

### Verifying Builds

To verify your builds worked correctly:

**Linux:**
```bash
file keylogger
# Should show: ELF 64-bit LSB executable, x86-64, version 1 (SYSV)
```

**Windows:**
```bash
file keylogger_x64.exe
# Should show: PE32+ executable (GUI) x86-64, for MS Windows
```

## Notes

- The Windows executable requires administrator privileges to monitor global keyboard events
- The Linux executable requires root privileges to access input device files
- Both versions create timestamped log files in the same directory as the executable
- The configuration file `keylogger.conf` works the same on both platforms