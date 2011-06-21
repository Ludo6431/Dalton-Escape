#ifndef _JEU_H
#define _JEU_H

#include "jeu/gui.h"
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
} JEU;

void jeu_ajout_nouveau(JEU *ctx, GtkWindow *fen);

#endif
