#ifndef _JE_CALLBACKS_H
#define _JE_CALLBACKS_H

#include "JE.h"

// menu
void    nouvelle_partie (GtkWidget *w, JE *ctx);
void    sauver_partie   (GtkWidget *w, JE *ctx);
void    charger_partie  (GtkWidget *w, JE *ctx);
void    quitter_partie  (GtkWidget *w, JE *ctx);

// action des boutons
void    action_pion     (GtkWidget *widget, JE *ctx);

#endif

