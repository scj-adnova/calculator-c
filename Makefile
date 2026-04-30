CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11
TARGET  = calculator

.PHONY: all clean

all: $(TARGET)

$(TARGET): calculator.c
	$(CC) $(CFLAGS) -o $(TARGET) calculator.c -lm

clean:
	rm -f $(TARGET)
