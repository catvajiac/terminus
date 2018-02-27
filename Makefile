CC=gcc
CFLAGS=-Wall -std=gnu99
SOURCES=$(wildcard *.c)
TARGETS=$(SOURCES:.c=)

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	@rm $(TARGETS)

.PHONY: clean all
