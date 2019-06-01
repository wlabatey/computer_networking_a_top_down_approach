CC ?= gcc
CFLAGS ?= -Wall -pedantic -Wextra -ggdb

abp: abp.c
	$(CC) $(CFLAGS) -o abp abp.c

.PHONY: clean
clean:
	rm -f *.o abp gn
