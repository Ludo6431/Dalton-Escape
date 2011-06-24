#ifndef _JOUEUR_H
#define _JOUEUR_H

#include <gtk/gtk.h>

typedef struct {
    char pseudo[20];
    GdkColor color;

    int score;
} JOUEUR;

void    joueur_init         (JOUEUR *pl, char *msg, char *dpseudo, GdkColor *dcolor, GtkWindow *top_win);

int     joueur_sauvegarder  (JOUEUR *pl, FILE *fd);
int     joueur_charger      (JOUEUR *pl, FILE *fd);

#endif

