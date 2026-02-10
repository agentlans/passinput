# Variables
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Werror -Wno-unused-parameter
LIB_NAME = libpassinput.a
OBJ = passinput.o
TARGET = my_app

# Default target
all: $(TARGET)

# Compile the Static Library
$(LIB_NAME): passinput.c passinput.h
	$(CC) $(CFLAGS) -c passinput.c -o $(OBJ)
	ar rcs $(LIB_NAME) $(OBJ)

# Compile the Application and link the library
$(TARGET): main.c $(LIB_NAME)
	$(CC) $(CFLAGS) main.c -L. -lpassinput -o $(TARGET)

# Clean build files
clean:
	rm -f $(OBJ) $(LIB_NAME) $(TARGET)

# Phony targets
.PHONY: all clean
