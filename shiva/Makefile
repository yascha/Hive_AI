CC=gcc
CFLAGS=-std=c99 
DEPS=types.h debug.h
OBJS=zobrist.o time.o main.o game.o board.o pieces.o isWin.o evaluate.o alphabeta.o clock.o

all : shiva

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shiva: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean :
	$(RM) *.o shiva
