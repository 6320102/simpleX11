# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -I/usr/include/X11 -L/usr/lib/x86_64-linux-gnu
LDFLAGS = -lX11 -lm

# Get a list of all C source files in the current directory
SRCS = $(wildcard *.c)

# Generate a list of executable files based on the source files
EXES = $(SRCS:.c=)

# Default target
all: $(EXES)

# Rule to create executable files
%: %.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

# Clean up build artifacts
clean:
	rm -f $(EXES) *.o

.PHONY: all clean