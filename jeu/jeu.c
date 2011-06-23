#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "jeu/jeu.h"

void jeu_ajout_nouveau(JEU *ctx, GtkWindow *fen) {
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

void jeu_empile_coup(JEU *ctx, EV *jeu) {
    ctx->pile_coups = g_slist_prepend(ctx->pile_coups, g_slice_dup(EV, jeu));
}

EV* jeu_depile_coup(JEU *ctx) {
    EV *ret = NULL;

    if(ctx->pile_coups) {
        ret = (EV *)ctx->pile_coups->data;
        ctx->pile_coups = g_slist_remove(ctx->pile_coups, ret); // remove the first element
    }

    return ret;
}

