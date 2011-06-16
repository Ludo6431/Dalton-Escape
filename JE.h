#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include "JE_joueur.h"
#include "JE_jeu.h"

typedef struct {
    GtkWidget *fenetre;
        GtkWidget *vbox;
            GtkWidget *menu;
            GtkWidget *aframe;
                GtkWidget *table;
                    GtkWidget *bt_cellules;
                    GtkWidget *bts_cases[9][9];
                    GtkWidget *lbl_J1, *score_J1;
                    GtkWidget *bt_sortie;
                    GtkWidget *lbl_J2, *score_J2;
            GtkWidget *lbl_statut;

    JE_joueur J1, J2;
    JE_jeu jeu;

    char *filename;
} JE;

GtkWidget *JE_nouveau(JE *ctx, GtkWindow *fen);

#endif

