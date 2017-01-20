CC=gcc
CFLAGS= -std=c11 -Werror -I. -Incurses
DEPS = ui.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gomoku: main.o ui.o
	gcc -o gomoku main.o ui.o -I. -lncurses

clean:
	rm -rf *.o
