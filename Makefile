CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c99 -pedantic
DEPS=String.c Object.c Array.c

all: main

main: main.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

test: simple_dump fmtjson

simple_dump: tests/simple_dump.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

fmtjson: tests/fmtjson.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm fmtjson simple_dump main


.PHONY: all clean test
