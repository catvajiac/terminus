CC=gcc
CFLAGS=-Wall -std=gnu99 -g
SOURCES=$(wildcard *.c)
LIBS=$(wildcard lib/*.c)
LIBOBJ=$(LIBS:.c=.o)
LIBHEAD=$(LIBS:.c=.o)
TARGETS=$(SOURCES:.c=)

all: $(TARGETS)

client: client.o $(LIBOBJ) $(LIBHEAD)
	$(CC) -o $@ $< $(LIBOBJ)

server: server.o $(LIBOBJ) $(LIBHEAD)
	$(CC) -o $@ $< $(LIBOBJ)

clean:
	@rm $(TARGETS)

.PHONY: clean all
