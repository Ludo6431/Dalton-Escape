#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editeur/editeur.h"

void editeur_ajout_nouveau(EDIT *ctx, GtkWindow *fen) {
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

void editeur_empile_coup(EDIT *ctx) {
    if(ctx->coups) {
        assert(ctx->coup_curr);

        GList *next;
        while((next = g_list_next(ctx->coup_curr))) {
            g_slice_free(EV, next->data);
            ctx->coups = g_list_delete_link(ctx->coups, next);
        }

        ctx->coups = g_list_append(ctx->coup_curr/* ctx->coups */, g_slice_dup(EV, &ctx->jeu));
        ctx->coup_curr = g_list_next(ctx->coup_curr);

        assert(!g_list_next(ctx->coup_curr));
    }
    else
        ctx->coup_curr = ctx->coups = g_list_append(NULL, g_slice_dup(EV, &ctx->jeu));

    // on met à jour l'interface
    if(g_list_previous(ctx->coup_curr))
        gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Undo"), TRUE);
    else
        gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Undo"), FALSE);
    gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Redo"), FALSE);
}

void editeur_annule_coup(EDIT *ctx) {
    if(!ctx->coups || !ctx->coup_curr || !g_list_previous(ctx->coup_curr))
        return;

    ctx->coup_curr = g_list_previous(ctx->coup_curr);

    memcpy(&ctx->jeu, ctx->coup_curr->data, sizeof(EV));

    // on met à jour l'interface
    if(g_list_previous(ctx->coup_curr))
        gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Undo"), TRUE);
    else
        gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Undo"), FALSE);
    gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Redo"), TRUE);
}

void editeur_refait_coup(EDIT *ctx) {
    if(!ctx->coups || !ctx->coup_curr || !g_list_next(ctx->coup_curr))
        return;

    ctx->coup_curr = g_list_next(ctx->coup_curr);

    memcpy(&ctx->jeu, ctx->coup_curr->data, sizeof(EV));

    // on met à jour l'interface
    gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Undo"), TRUE);
    if(g_list_next(ctx->coup_curr))
        gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Redo"), TRUE);
    else
        gtk_widget_set_sensitive(gtk_ui_manager_get_widget(ctx->gui.menu_manager, "/MainMenu/EditMenu/Redo"), FALSE);
}

