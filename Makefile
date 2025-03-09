CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
