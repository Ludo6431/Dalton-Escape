#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "JE/JE.h"

void JE_ajout_nouveau(JE *ctx, GtkWindow *fen) {
    GtkWidget *gui;
    GdkGeometry hints;

    // crée l'interface
    gui = gui_init(&ctx->gui, fen, (void *)ctx);
    gtk_container_add(GTK_CONTAINER(fen), gui);
    gtk_widget_show(gui);

    // limite le ratio de la fenêtre
    hints.min_aspect = 9.0/(13.0+1.5);
    hints.max_aspect = 9.0/(13.0+1.5);
    gtk_window_set_geometry_hints(GTK_WINDOW(fen), NULL, &hints, GDK_HINT_ASPECT);
}

