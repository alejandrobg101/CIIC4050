CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=200809L
TARGET = manager
SRCDIR = src
SOURCES = $(SRCDIR)/manager.c
BUILDDIR = build

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Build the manager executable
$(BUILDDIR)/$(TARGET): $(SOURCES) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $<

# Default target
all: $(BUILDDIR)/$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)

# Install manager to parent directory for easy access
install: $(BUILDDIR)/$(TARGET)
	cp $(BUILDDIR)/$(TARGET) ../

# Phony targets
.PHONY: all clean install
