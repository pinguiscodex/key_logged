# Silent Keylogger for Linux - Project Summary

## Overview
This project implements a silent keylogger for Linux that tracks every pressed key and writes it to a log file. When a mouse button or the Enter key is pressed, it creates a new line in the log file.

## Files Created

### Core Components
1. `keylogger.c` - Main source code for the keylogger
2. `keylogger` - Compiled executable (created after compilation)
3. `Makefile` - Build automation file
4. `keylog.txt` - Log file (created when keylogger runs)

### Documentation
5. `README.md` - Main documentation with installation and usage instructions
6. `TROUBLESHOOTING.md` - Detailed troubleshooting guide
7. `DESIGN.md` - Design decisions and architecture

### Automation Scripts
8. `install.sh` - Distribution-aware installation script
9. `test_keylogger.sh` - Comprehensive testing suite

### System Integration
10. `keylogger.service` - Systemd service file for system-wide installation

## Key Features

### Core Functionality
- Tracks all keyboard inputs with precision
- Tracks mouse button clicks separately
- Writes to timestamped local log files for each session
- Creates new lines on Enter key or mouse button press
- Intelligent modifier key tracking (Shift, Caps Lock, AltGr)
- Accurate character mapping based on keyboard state
- Proper handling of uppercase/lowercase letters
- Correct symbol mapping for shifted keys (e.g., @ when Shift+2)
- Completely silent operation with no console output
- Invisible background operation
- Undetectable by standard monitoring tools
- Robust error handling without user notification

### Cross-Distribution Compatibility
- Automatic detection of Linux distribution
- Package manager-specific installation commands
- Support for: Debian/Ubuntu, Red Hat/CentOS/Fedora, Arch Linux, openSUSE, Gentoo
- Fallback mechanisms for unknown distributions

### Installation Options
- User-level autostart (XDG autostart)
- System-wide systemd service
- Manual execution

### Security Considerations
- Requires input device access permissions
- Clear guidance on privilege escalation options
- Secure handling of sensitive log data

## Architecture

### Technical Implementation
- Written in C for efficiency and low-level access
- Uses Linux input subsystem (`/dev/input/event*`)
- Implements proper error handling
- Efficient event processing with `select()` system call

### Design Decisions
- Single executable with no external dependencies
- Local log storage in same directory
- Minimal resource usage
- Clean shutdown handling

## Testing Framework

### Automated Tests
- Binary existence verification
- Compilation validation
- Runtime functionality checks
- Permission verification
- Distribution compatibility checks

### Manual Testing Procedures
- Basic functionality test
- Autostart verification
- Log format validation
- Performance monitoring

## Deployment Methods

### Method 1: User Autostart (Recommended)
- Uses XDG autostart specification
- Runs with user privileges
- Easy to manage and uninstall

### Method 2: System Service
- Runs system-wide
- Requires root privileges
- More complex setup but broader access

### Method 3: Manual Execution
- For testing and debugging
- Direct control over execution

## Maintenance

### Updates
- Simple binary replacement
- Backward-compatible log format
- Clear upgrade procedures

### Monitoring
- Low resource footprint
- Standard process monitoring tools
- Built-in logging of start/stop times

## Compliance and Ethics

### Responsible Use
- Intended for authorized penetration testing
- Educational purposes only
- Compliance with applicable laws required
- Privacy considerations addressed

### Data Handling
- Sensitive information in logs
- Secure storage recommendations
- Proper disposal procedures

## Support

For issues with this keylogger:
1. Consult TROUBLESHOOTING.md
2. Verify system requirements
3. Check permissions
4. Review distribution-specific instructions

## Version Information
Version: 1.0
Last Updated: February 2026
Target: Linux systems with input subsystem support