CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c99 -pedantic

DEPS=String.c Object.c

all: main

main: main.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

test: simple_dump

simple_dump: tests/simple_dump.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^
