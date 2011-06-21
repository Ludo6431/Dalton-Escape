#ifndef _GUI_ACTION_H
#define _GUI_ACTION_H

#include "editeur/editeur.h"

// action des boutons
void        maj_etat            (JEU *ctx);

// glisser-deposer
void        debut_drag          (GtkWidget *widget, GdkDragContext *context, JEU *ctx);
gboolean    drop_drag           (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, JEU *ctx);
void        demande_donnee_drag (GtkWidget *widget, GdkDragContext *context, GtkSelectionData *selection_data, guint target_type, guint time, JEU *ctx);
void        recoit_donnee_drag  (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *selection_data, guint target_type, guint time, JEU *ctx);
void        fin_drag            (GtkWidget *widget, GdkDragContext *context, JEU *ctx);

// cliquer-cliquer
void        action_pion         (GtkWidget *widget, JEU *ctx);

#endif

