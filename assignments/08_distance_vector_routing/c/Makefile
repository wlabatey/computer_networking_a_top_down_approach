CC ?= gcc
CFLAGS ?= -Werror -Wextra -Wall -pedantic -ggdb
SOURCE_FILES ?= node0.c node1.c node2.c node3.c prog3.c

.DEFAULT_GOAL: dv

.PHONY: clean

dv: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o dv $(SOURCE_FILES)

clean:
	rm dv
