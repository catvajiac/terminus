CC=gcc
CFLAGS=-Wall -std=gnu99 -g -pthread
SOURCES=$(wildcard *.c)
LFLAGS=-pthread
LIBS=$(wildcard lib/*.c)
LIBOBJ=$(LIBS:.c=.o)
LIBHEAD=$(LIBS:.c=.o)
TARGETS=$(SOURCES:.c=)

all: $(TARGETS) menu

client: client.o $(LIBOBJ) $(LIBHEAD)
	$(CC) -o $@ $< $(LIBOBJ) $(LFLAGS)

server: server.o $(LIBOBJ) $(LIBHEAD)
	$(CC) -o $@ $< $(LIBOBJ)

clean:
	@rm $(TARGETS) && make -C ./apps/ clean

menu:
	make -C ./apps/

.PHONY: clean all menu
