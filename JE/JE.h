#ifndef _JE_H
#define _JE_H

#include "JE/gui.h"
#include "JE/joueur.h"
#include "JE/jeu.h"

// contexte général du jeu
typedef struct {
    // IHM
    JE_gui gui;
    int dx, dy; // utilisé pour le glisser-déposer

    // les 2 joueurs
    JE_joueur J1, J2;

    // l'état du jeu
    JE_jeu jeu;

    // fichier où la partie est sauvegardée
    char *filename;
} JE;

void JE_ajout_nouveau(JE *ctx, GtkWindow *fen);

#endif
