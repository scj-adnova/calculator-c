CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11
TARGET  = calculator

.PHONY: all clean test

all: $(TARGET)

test: $(TARGET)
	@bash tests/run_tests.sh

$(TARGET): calculator.c
	$(CC) $(CFLAGS) -o $(TARGET) calculator.c -lm

clean:
	rm -f $(TARGET)
