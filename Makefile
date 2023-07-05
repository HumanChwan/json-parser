CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c99 -pedantic

DEPS=String.c Object.c Array.c

all: main

main: main.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

test: simple_dump load_n_dump

simple_dump: tests/simple_dump.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

load_n_dump: tests/load_n_dump.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^
