CC=gcc
CFLAGS=-Wall
RM=rm
OPT=-O3
BIN1=wadDecode
BIN2=wadEncode

all: src/decoder.c
	@$(CC) $(CFLAGS) $(OPT) -Wno-unused -o $(BIN1) src/decoder.c src/bmp.c src/utils.c
	@$(CC) $(CFLAGS) $(OPT) -Wno-unused -o $(BIN2) src/encoder.c src/bmp.c src/utils.c src/tga.c  -lm
clean:
	@$(RM) $(BIN1)	
	@$(RM) $(BIN2)