# Makefile for compiling mergesort, countfile, and distributedsort

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -lm

# Source files
SRCS = mergesort.c countfile.c distributedsort.c

# Output executable
TARGET = sort_program

# Default target to build the program
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up object files and the executable
clean:
	rm -f $(TARGET)
