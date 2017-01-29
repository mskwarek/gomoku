CC=gcc
CFLAGS=-Werror -I. -Incurses -Iinclude
#DEPS=ui.h
LDFLAGS=-lncurses
OBJDIR=obj
SRCDIR=src 
INC=-Iinclude

SOURCES=main.c $(SRCDIR/*)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=gomoku

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf obj/*
	rm -rf bin
