# Makefile for Lab0 countwords
# CSE111 Fall 2020
# Marques Reimann

# Use g++ for compiler
CC = g++

# Compiler flags:
# -g 	adds debbugging info to executable
# -wall turns on most compiler warnings
CFLAGS = -g -Wall

# files to link: (none)
FLAGS = 

# the name to use for both the target source file, and the output file:
TARGET = countwords

all: $(TARGET)

# Removes the program
clean:
	-rm $(TARGET)

# Makes and runs the program
run:
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LFLAGS)
	./$(TARGET)

# Default: Makes the program
$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LFLAGS)
