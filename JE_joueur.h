#ifndef _JE_JOUEUR_H
#define _JE_JOUEUR_H

#include <gtk/gtk.h>

typedef struct {
    char pseudo[20];
    GdkColor color;

    int score;
} JE_joueur;

void joueur_init(JE_joueur *pl, char *msg, char *dpseudo, GtkWindow *top_win);

void joueur_sauver(JE_joueur *pl, FILE *fd);
void joueur_charger(JE_joueur *pl, FILE *fd);

#endif

