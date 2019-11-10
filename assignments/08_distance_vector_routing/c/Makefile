CC = gcc
CFLAGS = -Werror -Wextra -Wall -pedantic -ggdb

.DEFAULT_GOAL: dv

.PHONY: clean cleanall

node0.o: node0.c node0.h
	$(CC) $(CFLAGS) -c node0.c

node1.o: node1.c node1.h
	$(CC) $(CFLAGS) -c node1.c

node2.o: node2.c node2.h
	$(CC) $(CFLAGS) -c node2.c

node3.o: node3.c node3.h
	$(CC) $(CFLAGS) -c node3.c

prog3.o: prog3.c prog3.h
	$(CC) $(CFLAGS) -c prog3.c

dv: node0.o node1.o node2.o node3.o prog3.o
	$(CC) $(CFLAGS) node0.o node1.o node2.o node3.o prog3.o -o dv

clean:
	rm -f dv

cleanall: clean
	rm -f node0.o node1.o node2.o node3.o prog3.o
