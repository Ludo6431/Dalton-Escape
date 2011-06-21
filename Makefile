.PHONY:all clean

EXEC := dalton-escape
OBJS := jeu.o jeu/jeu.o jeu/gui.o jeu/gui_action.o jeu/gui_menu.o

OBJS += libevasion/evasion.o commun/joueur.o commun/outils.o

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

