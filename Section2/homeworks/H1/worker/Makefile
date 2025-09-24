CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=200809L
TARGET = worker
SRCDIR = src
SOURCES = $(SRCDIR)/worker.c
BUILDDIR = build

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Build the worker executable
$(BUILDDIR)/$(TARGET): $(SOURCES) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $<

# Default target
all: $(BUILDDIR)/$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)

# Install worker to parent directory for easy access
install: $(BUILDDIR)/$(TARGET)
	cp $(BUILDDIR)/$(TARGET) ../

# Phony targets
.PHONY: all clean install
