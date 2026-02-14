# Silent Keylogger for Cross-Platform (Linux/Windows)

This is a completely silent keylogger that tracks every pressed key and writes it to a timestamped log file. When a mouse button or the Enter key is pressed, it creates a new line in the log file. The keylogger is now compatible with both Linux and Windows operating systems.

## Features

- **Completely Silent Operation**: No console output or visible indicators during operation
- **Cross-Platform Compatibility**: Works on both Linux and Windows systems
- **Timestamped Log Files**: Creates log files named `keylog_YYYYMMDD_HHMMSS.txt` for each session
- **Basic Character Mapping**: Logs characters as they are pressed without complex formatting
- **Mouse Event Detection**: Creates new lines when mouse buttons are clicked
- **Enter Key Recognition**: Creates new lines when Enter key is pressed
- **Invisible Background Operation**: Runs undetectably in the background
- **Autostart Capability**: Can be added to system autostart applications
- **Robust Error Handling**: Handles errors silently without user notification

## Files Included

- `keylogger.c`: Source code for the keylogger
- `Makefile`: For compiling the keylogger
- `keylogger.service`: Systemd service file
- `install.sh`: Installation script
- `README.md`: This file

## Prerequisites

The keylogger requires the following packages to compile and run:
- `build-essential` (gcc compiler)
- `libinput-dev` (input device headers)
- Root access to read input devices

## Compilation

### Linux
```bash
make
```

For systems without build-essential:
```bash
# Debian/Ubuntu
sudo apt-get install build-essential

# RHEL/CentOS/Fedora
sudo yum install gcc make
# or for newer Fedora
sudo dnf install gcc make

# Arch Linux
sudo pacman -S gcc make
```

### Windows
For Windows, you can compile using MinGW-w64 or Visual Studio. The code uses Windows API hooks for keylogging functionality.

## Building Executables

### Linux
```bash
make
```

This creates the Linux executable `keylogger`.

### Windows
To build for Windows from Linux, you need MinGW-w64 cross-compiler:

```bash
# Install MinGW-w64 cross-compiler
# On Ubuntu/Debian:
sudo apt-get install gcc-mingw-w64

# On CentOS/RHEL/Fedora:
sudo yum install mingw64-gcc mingw32-gcc

# On Arch Linux:
sudo pacman -S mingw-w64-gcc

# Build Windows executable
./build_windows.sh
```

This creates the Windows executable `keylogger.exe`.

### Comprehensive Build Scripts

For more advanced building, we provide comprehensive build scripts:

```bash
# Make all scripts executable
chmod +x build_*.sh

# Build for Linux only
./build_linux.sh

# Build for Windows (cross-compile)
./build_windows_full.sh

# Build for all platforms with one command
./build_all.sh --all
```

For detailed build instructions, see [BUILDING.md](BUILDING.md).

## Manual Execution

### Linux
```bash
./keylogger
```

Note: The program needs to run with sufficient privileges to access input devices. On most systems, this means running as root or having appropriate permissions.

### Windows
On Windows, the executable needs to be run with administrator privileges to access global keyboard events. Double-click the executable or run it from an elevated command prompt.

## Adding to Autostart

### Linux

#### Method 1: Using the install script (recommended)

```bash
./install.sh
```

This will:
1. Install required dependencies based on your distribution
2. Compile the keylogger
3. Add it to your autostart applications

#### Method 2: Manual addition to autostart

Create a desktop entry file in `~/.config/autostart/keylogger.desktop`:

```ini
[Desktop Entry]
Type=Application
Name=Keylogger
Exec=/full/path/to/keylogger
Hidden=true
NoDisplay=true
X-GNOME-Autostart-enabled=true
```

#### Method 3: Using systemd service (for system-wide installation)

1. Copy the executable to `/usr/local/bin/`:
   ```bash
   sudo cp keylogger /usr/local/bin/
   sudo chmod +x /usr/local/bin/keylogger
   ```

2. Copy the service file to `/etc/systemd/system/`:
   ```bash
   sudo cp keylogger.service /etc/systemd/system/
   ```

3. Enable and start the service:
   ```bash
   sudo systemctl enable keylogger.service
   sudo systemctl start keylogger.service
   ```

### Windows

On Windows, you can add the executable to autostart by placing it in the Startup folder or by adding a registry entry. For the registry method, add the executable path to:
`HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run`

## Platform Compatibility

This keylogger is designed to work on both Linux and Windows platforms:

### Linux:
- **Debian/Ubuntu**: Uses APT package manager
- **Red Hat/CentOS/Fedora**: Uses YUM/DNF package manager
- **Arch Linux**: Uses Pacman package manager
- **openSUSE**: Uses ZYPPER package manager
- **Gentoo**: Compatible with Portage

### Windows:
- **Windows 7/8/10/11**: Uses Windows API hooks for keylogging
- Requires administrator privileges for global keyboard monitoring

The installation script automatically detects your distribution and installs the appropriate dependencies.

## Available Executables

Pre-built executables are available for both platforms:
- `keylogger` - Linux executable
- `keylogger.exe` - Windows executable

## Log File Location

By default, the log file (`keylog_YYYYMMDD_HHMM.txt`) will be created in the same directory as the executable. You can customize this location by creating a `keylogger.conf` configuration file.

### Linux
The log file will be created in the same directory as the executable.

### Windows
The log file will be created in the same directory as the executable.

## Configuration

The keylogger can be configured using a `keylogger.conf` file in the same directory as the executable. The configuration file supports the following option:

- `log_directory`: Specify a custom directory for log files (leave empty or use '.' for executable directory)

The keylogger will automatically create the specified directory if it doesn't exist and will check for write permissions. If the custom directory is not accessible, it will fall back to using the executable's directory and display a warning.

Example configuration file:
```
# Log Directory - Specify where log files should be stored
# Leave empty or use '.' to store in the same directory as the executable
# The keylogger will create the directory if it doesn't exist and has permission
# Example: /var/log/keylogger/ or /home/user/logs/
log_directory=
```

## Stopping the Keylogger

To stop the keylogger, find its process ID and kill it:

```bash
ps aux | grep keylogger
kill [PID]
```

Or if running as a systemd service:
```bash
sudo systemctl stop keylogger
```


## Security Considerations

- The program requires access to `/dev/input/event*` devices
- On most systems, this requires running as root or adding your user to the `input` group
- To add your user to the input group (not recommended for security):
  ```bash
  sudo gpasswd -a $USER input
  # Then log out and back in
  ```

## Troubleshooting

- If the program fails to detect input devices, ensure you have appropriate permissions
- If compilation fails, ensure build tools are installed
- If autostart doesn't work, check that your desktop environment supports XDG autostart

## Important Notes

- This tool requires root privileges to access input devices
- Use responsibly and in compliance with applicable laws
- The logged data contains sensitive information - handle securely
- This tool is intended for educational purposes and authorized penetration testing