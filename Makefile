CC=gcc
CFLAGS=-Wall
RM=rm
OPT=-O3
BIN=wadParse

all: src/main.c
	@$(CC) $(CFLAGS) $(OPT) -Wno-unused -o $(BIN) src/main.c src/bmp.c src/utils.c
clean:
	@$(RM) wadParse