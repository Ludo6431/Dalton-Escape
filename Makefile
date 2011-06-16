.PHONY:all clean

EXEC := evasion
OBJS := main.o JE.o JE_joueur.o JE_jeu.o JE_callbacks.o

CC = gcc
CFLAGS = -Wall -g `pkg-config gtk+-2.0 --cflags`
LDFLAGS = `pkg-config gtk+-2.0 --libs`

all:$(EXEC)

clean:
	rm $(EXEC) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

simple.o:

