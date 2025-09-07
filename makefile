TARGET = compilador

CC = gcc
CFLAGS = -Wall -Wno-unused-result -g -Og

SRCS = compilador.c globals.c lexer.c parser.c

$(TARGET):
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lm

.PHONY: clean
clean:
	rm -f $(TARGET)