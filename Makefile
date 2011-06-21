.PHONY:all clean

# jeu
EXEC_JEU := dalton-escape
OBJS_JEU := jeu.o jeu/jeu.o jeu/gui.o jeu/gui_action.o jeu/gui_menu.o

# editeur
EXEC_EDITEUR := dalton-escape-editeur
OBJS_EDITEUR := editeur.o editeur/editeur.o editeur/gui.o editeur/gui_action.o editeur/gui_menu.o

# en commun
OBJS_COMMUN := libevasion/evasion.o commun/joueur.o commun/outils.o

# -------- pas besoin d'éditer après cette ligne ---------

EXECS := $(EXEC_JEU) $(EXEC_EDITEUR)
OBJS := $(OBJS_JEU) $(OBJS_EDITEUR) $(OBJS_COMMUN)

OBJS_JEU += $(OBJS_COMMUN)
OBJS_EDITEUR += $(OBJS_COMMUN)

CC = gcc
CFLAGS = -Wall -g -I. `pkg-config gtk+-2.0 --cflags`
LDFLAGS = `pkg-config gtk+-2.0 --libs`

all:$(EXECS)

clean:
	rm $(EXECS) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC_JEU):$(OBJS_JEU) $(OBJS_COMMUN)
	$(CC) $(LDFLAGS) -o $@ $^

$(EXEC_EDITEUR):$(OBJS_EDITEUR) $(OBJS_COMMUN)
	$(CC) $(LDFLAGS) -o $@ $^

