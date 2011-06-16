.PHONY:all clean

EXEC := evasion
OBJS := main.o JE/JE.o JE/joueur.o JE/jeu.o JE/gui.o JE/gui_action.o JE/gui_menu.o JE/outils.o

CC = gcc
CFLAGS = -Wall -g -I. `pkg-config gtk+-2.0 --cflags`
LDFLAGS = `pkg-config gtk+-2.0 --libs`

all:$(EXEC)

clean:
	rm $(EXEC) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

simple.o:

