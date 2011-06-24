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
    GList *coups; // pour annuler les derniers coups
    GList *coup_curr;   // coup_curr pointe toujours vers la version la plus à jour de jeu

    // fichier où la partie est sauvegardée
    char *filename;
} EDIT;

void    editeur_ajout_nouveau   (EDIT *ctx, GtkWindow *fen);

void    editeur_empile_coup     (EDIT *ctx);
void    editeur_annule_coup     (EDIT *ctx);
void    editeur_refait_coup     (EDIT *ctx);
void    editeur_vide_coups      (EDIT *ctx);

#endif
