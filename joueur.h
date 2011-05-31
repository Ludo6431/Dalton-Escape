#ifndef _JOUEUR_H
#define _JOUEUR_H

#include <gtk/gtk.h>

typedef struct {
    char pseudo[20];
    GdkColor color;

    int score;
} JE_joueur;

void player_init(JE_joueur *pl, char *msg, char *dpseudo, GtkWindow *top_win);

#endif

