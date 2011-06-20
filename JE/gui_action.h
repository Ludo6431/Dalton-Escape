#ifndef _JE_GUI_ACTION_H
#define _JE_GUI_ACTION_H

#include "JE/JE.h"

// action des boutons
void    maj_etat        (JE *ctx);

void        debut_drag          (GtkWidget *widget, GdkDragContext *context, JE *ctx);
gboolean    drop_drag           (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, JE *ctx);
void        demande_donnee_drag (GtkWidget *widget, GdkDragContext *context, GtkSelectionData *selection_data, guint target_type, guint time, JE *ctx);
void        recoit_donnee_drag  (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *selection_data, guint target_type, guint time, JE *ctx);
void        fin_drag            (GtkWidget *widget, GdkDragContext *context, JE *ctx);

void    action_pion     (GtkWidget *widget, JE *ctx);

#endif

