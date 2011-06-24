#ifndef _GUI_MENU_H
#define _GUI_MENU_H

#include "editeur/editeur.h"

// Game Menu
void    nouvelle_partie     (GtkWidget *w, EDIT *ctx);
void    sauvegarder_partie  (GtkWidget *w, EDIT *ctx);
void    charger_partie      (GtkWidget *w, EDIT *ctx);
void    quitter_partie      (GtkWidget *w, EDIT *ctx);
void    afficher_apropos    (GtkWidget *w, EDIT *ctx);

// Edit Menu
void    annuler_coup        (GtkWidget *w, EDIT *ctx);
void    refaire_coup        (GtkWidget *w, EDIT *ctx);

#endif

