CC = gcc
CFLAGS = -Wall -O2
LIBS = -lrt

all: keylogger

keylogger: keylogger.c
	$(CC) $(CFLAGS) -o keylogger keylogger.c $(LIBS)

cross: keylogger_cross_platform.c
	$(CC) $(CFLAGS) -o keylogger_cross keylogger_cross_platform.c $(LIBS)

clean:
	rm -f keylogger keylogger_cross

install: keylogger
	sudo cp keylogger /usr/local/bin/
	sudo chmod +x /usr/local/bin/keylogger
	# Copy config file to the same directory as the executable if it doesn't exist
	if [ ! -f /usr/local/bin/keylogger.conf ]; then \
		sudo cp keylogger.conf /usr/local/bin/; \
	fi

.PHONY: all clean install cross