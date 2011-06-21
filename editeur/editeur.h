#ifndef _EDITEUR_H
#define _EDITEUR_H

#include "editeur/gui.h"
#include "commun/joueur.h"
#include "libevasion/evasion.h"

// contexte général du jeu
typedef struct {
    // IHM
    GUI gui;
    int dx, dy; // utilisé pour le glisser-déposer

    // les 2 joueurs
    JOUEUR J1, J2;

    // l'état du jeu
    EV jeu;

    // fichier où la partie est sauvegardée
    char *filename;
} EDIT;

void editeur_ajout_nouveau(EDIT *ctx, GtkWindow *fen);

#endif
