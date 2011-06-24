#ifndef _GUI_MENU_H
#define _GUI_MENU_H

#include "jeu/jeu.h"

// Game Menu
void    nouvelle_partie     (GtkWidget *w, JEU *ctx);
void    afficher_scores     (GtkWidget *w, JEU *ctx);
void    sauvegarder_partie  (GtkWidget *w, JEU *ctx);
void    charger_partie      (GtkWidget *w, JEU *ctx);
void    quitter_partie      (GtkWidget *w, JEU *ctx);
void    afficher_apropos    (GtkWidget *w, JEU *ctx);

// Edit Menu
void    annuler_coup        (GtkWidget *w, JEU *ctx);
void    refaire_coup        (GtkWidget *w, JEU *ctx);

#endif

