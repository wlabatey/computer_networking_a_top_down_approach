CC ?= gcc
CFLAGS ?= -Werror -Wextra -Wall -pedantic -ggdb

.DEFAULT_GOAL := all

.PHONY: all
all: abp gbn

abp: abp.c
	$(CC) $(CFLAGS) -o abp abp.c

gbn: gbn.c
	$(CC) $(CFLAGS) -o gbn gbn.c

.PHONY: clean
clean:
	rm -f *.o abp gbn
