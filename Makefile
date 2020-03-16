all: particles

CC=gcc

LIBS=-lncurses -lm
CFLAGS=

FLAGS=-O3 -pipe -s -ansi -pedantic
DEBUGFLAGS=-Og -pipe -g -ansi -pedantic

INPUT=particles.c
OUTPUT=particles

INSTALLDIR=$(HOME)/.local/bin

RM=/bin/rm

particles:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) $(FLAGS)

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) $(DEBUGFLAGS)

install:
	test -d $(INSTALLDIR) || mkdir -p $(INSTALLDIR)
	install -pm 755 $(OUTPUT) $(INSTALLDIR)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
