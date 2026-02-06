CC = gcc
CFLAGS = -Wall -O2
LIBS = -lrt

all: keylogger

keylogger: keylogger.c
	$(CC) $(CFLAGS) -o keylogger keylogger.c $(LIBS)

clean:
	rm -f keylogger

install: keylogger
	sudo cp keylogger /usr/local/bin/
	sudo chmod +x /usr/local/bin/keylogger

.PHONY: all clean install