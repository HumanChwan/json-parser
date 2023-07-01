CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c99 -pedantic

all: main

main: main.c String.c
	$(CC) $(CFLAGS) -o $@ $^
