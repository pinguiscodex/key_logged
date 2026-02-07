# Keylogger Troubleshooting Guide

## Common Issues and Solutions

### 1. Silent Operation Expected
**Problem**: Keylogger runs but produces no console output
**Solution**:
- This is expected behavior - the keylogger operates completely silently
- Check for timestamped log files in the same directory: `ls -la keylog_*.txt`

### 2. Permission Denied Errors
**Problem**: Keylogger fails to start with permission errors
**Solution**:
- Run with sudo: `sudo ./keylogger`
- Or add your user to the input group: `sudo gpasswd -a $USER input` (then log out and back in)

### 3. Cannot Find Input Devices
**Problem**: Keylogger silently exits without creating a log file
**Solution**:
- Ensure you have access to `/dev/input/event*` devices
- Check permissions with: `ls -la /dev/input/event*`
- Verify input devices exist: `cat /proc/bus/input/devices`

### 3. Compilation Errors
**Problem**: Make fails with compilation errors
**Solution**:
- Install build essentials: `sudo apt-get install build-essential` (on Debian/Ubuntu)
- Or equivalent for your distribution

### 4. Autostart Not Working
**Problem**: Keylogger doesn't start automatically after login
**Solution**:
- Verify the desktop entry file exists: `~/.config/autostart/keylogger.desktop`
- Check that the Exec path is correct in the desktop file
- Ensure the keylogger binary is executable

### 5. Log File Not Created
**Problem**: No keylog.txt file is created
**Solution**:
- Check permissions in the directory where keylogger is located
- Verify the executable has write permissions to its directory
- Run with elevated privileges if needed

## Distribution-Specific Instructions

### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install build-essential
```

### Red Hat/CentOS/Fedora
```bash
# For older versions
sudo yum install gcc make

# For newer versions (Fedora 22+)
sudo dnf install gcc make
```

### Arch Linux
```bash
sudo pacman -S gcc make
```

### openSUSE
```bash
sudo zypper install gcc make
```

### Gentoo
```bash
emerge -av sys-devel/gcc sys-devel/make
```

## Security Considerations

### Device Permissions
The keylogger requires access to input devices which typically requires root privileges. Here are options to grant access:

1. **Run as root** (least secure):
   ```bash
   sudo ./keylogger
   ```

2. **Add user to input group** (moderate risk):
   ```bash
   sudo gpasswd -a $USER input
   # Log out and back in
   ```

3. **Use udev rules** (most secure but complex):
   Create `/etc/udev/rules.d/99-keylogger.rules`:
   ```
   SUBSYSTEM=="input", GROUP="input", MODE="0640"
   KERNEL=="event*", SUBSYSTEM=="input", GROUP="input", MODE="0644"
   ```
   
   Then add user to input group:
   ```bash
   sudo gpasswd -a $USER input
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```


## Performance Monitoring

### Resource Usage
The keylogger should have minimal resource usage:
- CPU: < 1% under normal conditions
- Memory: < 5MB
- Disk: Minimal (only writes when keys are pressed)

### Process Monitoring
Monitor the keylogger process:
```bash
# Check if running
pgrep keylogger

# Resource usage
top -p $(pgrep keylogger)
```

## Logging Format

The keylogger follows this format:
- Regular keys: Printed as-is
- Special keys: Enclosed in brackets (e.g., [ENTER], [TAB], [F1])
- Mouse clicks: Appear as new lines with [MOUSE LEFT] etc.
- Timestamps: Added when logger starts/stops

## Updating the Keylogger

To update to a new version:
1. Stop the current process: `pkill keylogger`
2. Backup your log file: `cp keylog.txt keylog_backup.txt`
3. Replace the binary with the new version
4. Restart or reinstall autostart

## Uninstallation

### Remove Autostart
```bash
rm ~/.config/autostart/keylogger.desktop
```

### Remove Binary
```bash
rm /usr/local/bin/keylogger  # if installed system-wide
rm /path/to/keylogger        # if in custom location
```

### Remove Log File
```bash
rm keylog.txt
```

## Legal and Ethical Considerations

- Only use this tool on systems you own or have explicit permission to monitor
- Be aware of privacy laws in your jurisdiction
- Inform users if deployed on shared systems
- Secure log files appropriately as they contain sensitive information