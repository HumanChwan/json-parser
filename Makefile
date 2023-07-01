CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c99 -pedantic

all: main

main: main.c String.c Object.c
	$(CC) $(CFLAGS) -D_DEV -o $@ $^
