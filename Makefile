CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -D_XOPEN_SOURCE_EXTENDED
LDLIBS = -lncursesw

pong: pong.c bibtela.c bibtela.h
	$(CC) $(CFLAGS) pong.c bibtela.c -o pong $(LDLIBS)

clean:
	rm -f pong
