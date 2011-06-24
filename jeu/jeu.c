#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jeu/jeu.h"

void jeu_ajout_nouveau(JEU *ctx, GtkWindow *fen) {
    GtkWidget *gui;
    GdkGeometry hints;

    // met à jour la graine du générateur pseudo-aléatoire
    srand(time(NULL));

    // crée l'interface
    gui = gui_init(&ctx->gui, fen, (void *)ctx);
    gtk_container_add(GTK_CONTAINER(fen), gui);
    gtk_widget_show(gui);

    // limite le ratio de la fenêtre
    hints.min_aspect = 9.0/(13.0+1.5);
    hints.max_aspect = 9.0/(13.0+1.5);
    gtk_window_set_geometry_hints(GTK_WINDOW(fen), NULL, &hints, GDK_HINT_ASPECT);
}

void jeu_empile_coup(JEU *ctx) {
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

void jeu_annule_coup(JEU *ctx) {
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

void jeu_refait_coup(JEU *ctx) {
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

void jeu_vide_coups(JEU *ctx) {
    if(!ctx->coups)
        return;

    assert(ctx->coup_curr);

    void slice_free(gpointer data, gpointer user) {
        g_slice_free(EV, data);
    }
    g_list_foreach(ctx->coups, slice_free, NULL);
    g_list_free(ctx->coups);

    ctx->coups = ctx->coup_curr = NULL;
}

