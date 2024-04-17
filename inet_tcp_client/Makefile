CFLAGS := -Wall -Wextra -g
TARGET := main

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): main.c
	gcc $(CFLAGS) $< -o $@