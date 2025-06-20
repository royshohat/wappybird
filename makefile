# Makefile for the 2D Multiplayer Game

# 1. Compiler and Flags
# ---------------------
CC = gcc
# CFLAGS: Flags for compilation.
# -Iinclude tells GCC to look for header files in the 'include' directory.
# -g adds debugging symbols (for GDB).
# -Wall turns on all major warnings.
# -Wextra turns on even more warnings.
# -std=c11 specifies the C standard.
CFLAGS = -Iinclude -g -Wall -Wextra -std=c11
# LDFLAGS: Flags for linking.
LDFLAGS =

# 2. Directories
# --------------
SRCDIR = src
INCDIR = include
BUILDDIR = build
BINDIR = bin

# Create lists of source files for each component.
# The wildcard function automatically finds all .c files in a directory.
COMMON_SRCS = $(wildcard $(SRCDIR)/common/*.c)
NET_SRCS = $(wildcard $(SRCDIR)/networking/*.c)
GRAPHICS_SRCS = $(wildcard $(SRCDIR)/graphics/*.c)
UTILS_SRCS = $(wildcard $(SRCDIR)/utils/*.c)
SERVER_SRCS = $(wildcard $(SRCDIR)/server/*.c)
CLIENT_SRCS = $(wildcard $(SRCDIR)/client/*.c)

# 3. Object Files
# ---------------
# This converts the .c source file lists into lists of .o object files
# that will be placed in the BUILDDIR.
COMMON_OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(COMMON_SRCS))
NET_OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(NET_SRCS))
GRAPHICS_OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(GRAPHICS_SRCS))
UTILS_OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(UTILS_SRCS))
SERVER_OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(CLIENT_SRCS))

# We need SDL3 for the client. This uses the `sdl3-config` tool to get
# the correct compiler and linker flags.
SDL_CFLAGS = $(shell sdl3-config --cflags)
SDL_LIBS = $(shell sdl3-config --libs)

# 4. Targets (The Recipes)
# ------------------------

# The 'all' target is the default. Typing 'make' will run this.
# It depends on the 'client' and 'server' targets.
all: client server

# Recipe to build the server executable.
# It depends on its own object files plus the common, networking, and utils object files.
server: $(SERVER_OBJS) $(COMMON_OBJS) $(NET_OBJS) $(UTILS_OBJS)
	@echo "Linking server..."
	@mkdir -p $(BINDIR) # Ensure the bin directory exists
	$(CC) $(LDFLAGS) $^ -o $(BINDIR)/server

# Recipe to build the client executable.
# It depends on its own object files plus all the other components.
# Note that we add the SDL flags here for both compiling and linking.
client: $(CLIENT_OBJS) $(COMMON_OBJS) $(NET_OBJS) $(GRAPHICS_OBJS) $(UTILS_OBJS)
	@echo "Linking client..."
	@mkdir -p $(BINDIR) # Ensure the bin directory exists
	$(CC) $(LDFLAGS) $^ -o $(BINDIR)/client $(SDL_LIBS)

# This is a generic "pattern rule". It's the most important part.
# It tells make how to create ANY .o file in the BUILDDIR from a .c file
# in the SRCDIR.
# $< is the first dependency (the .c file)
# $@ is the target (the .o file)
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D) # Ensure the subdirectory in 'build' exists
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

# The 'clean' target is for cleaning up. It removes the build and bin directories.
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILDDIR) $(BINDIR)

# A "phony" target is one that doesn't represent a real file.
.PHONY: all clean server client
