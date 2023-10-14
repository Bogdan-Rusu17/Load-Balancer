CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g
LOAD=load_balancer
SERVER=server

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o hashtable.o linked_list.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -FSr 312CA_RusuBogdan_Tema2.zip README.md Makefile *.c *.h

clean:
	rm -f *.o tema2 *.h.gch
