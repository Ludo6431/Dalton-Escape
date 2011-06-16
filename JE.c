#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JE_callbacks.h"

#include "JE.h"

static GtkActionEntry menu_entries[] = {
    /* name, stock id, label, accelerator, tooltip, callback */
    {"GameMenuAction", NULL, "_Game"},
    {"NewAction", GTK_STOCK_NEW, "_New", "<control>N", "Start new game", G_CALLBACK(nouvelle_partie)},
    {"SaveAction", GTK_STOCK_SAVE_AS, "_Save", "<control>S", "Save game", G_CALLBACK(sauver_partie)},
    {"LoadAction", GTK_STOCK_OPEN, "L_oad", "<control>O", "Load saved game", G_CALLBACK(charger_partie)},
    {"QuitAction", GTK_STOCK_QUIT, "_Quit", "<control>Q", "Quit game", G_CALLBACK(quitter_partie)},

    {"EditMenuAction", NULL, "E_dit"},
    {"UndoAction", GTK_STOCK_UNDO, "Undo", "<control>Z", "Undo last action", NULL},
    {"SettingsAction", GTK_STOCK_PREFERENCES, "Settings", NULL, "Save game", NULL},

    {"HelpMenuAction", NULL, "_Help"},
    {"ContentsAction", GTK_STOCK_DIALOG_QUESTION, "Contents", "F1", "Manual of the game", NULL},
    {"AboutAction", GTK_STOCK_ABOUT, "About", NULL, "About the game", NULL},
};

GtkWidget *JE_nouveau(JE *ctx, GtkWindow *fen) {
    GError *error;
    GdkGeometry hints;
    int i;
    int j;

    // création du style pour les différents boutons
    GtkStockItem items[] = {
        { "case-J1", "", 0, 0, "" },
        { "case-J2", "", 0, 0, "" },
        { "case-gardien", "", 0, 0, "" }
    };
    gtk_stock_add(items, G_N_ELEMENTS(items));
    gtk_rc_parse("style.rc");

    // on garde au chaud la fenêtre où on est
    ctx->fenetre = GTK_WIDGET(fen);

    // limite le ratio de la fenêtre
    hints.min_aspect = 9.0/(13.0+1.5);
    hints.max_aspect = 9.0/(13.0+1.5);
    gtk_window_set_geometry_hints(GTK_WINDOW(ctx->fenetre), NULL, &hints, GDK_HINT_ASPECT);

    // vbox principale
    ctx->vbox = gtk_vbox_new(FALSE, 0);

        // menu (TODO: mettre ça dans deux fichiers JE_menu.{c,h})
        GtkActionGroup *action_group;
        GtkUIManager *menu_manager;
        action_group = gtk_action_group_new("MenuActions");
        gtk_action_group_add_actions(action_group, menu_entries, G_N_ELEMENTS(menu_entries), ctx);
        menu_manager = gtk_ui_manager_new();
        gtk_ui_manager_insert_action_group(menu_manager, action_group, 0);
        error = NULL;
        gtk_ui_manager_add_ui_from_file(menu_manager, "menu.ui", &error);
        if(error) {
            g_message("building menus failed: %s", error->message);
            g_error_free(error);
        }
        gtk_window_add_accel_group(GTK_WINDOW(ctx->fenetre), gtk_ui_manager_get_accel_group(menu_manager));
        ctx->menu = gtk_ui_manager_get_widget(menu_manager, "/MainMenu");
        gtk_box_pack_start(GTK_BOX(ctx->vbox), ctx->menu, FALSE, FALSE, 0);
        gtk_widget_show(ctx->menu);

        // aspect frame (frame de ratio constant pour être sûr que les cases restent carrées)
        ctx->aframe = gtk_aspect_frame_new(NULL, 0.5, 0.5, 9.0/13.0, 0);
        gtk_box_pack_start(GTK_BOX(ctx->vbox), ctx->aframe, TRUE, TRUE, 0);

            // la table qui contient le plateau de jeu
            ctx->table = gtk_table_new(13, 9, TRUE);
            gtk_container_add(GTK_CONTAINER(ctx->aframe), ctx->table);

                // cellules
                ctx->bt_cellules = gtk_button_new_with_label("Cellules");
                g_signal_connect(G_OBJECT(ctx->bt_cellules), "clicked", G_CALLBACK(action_pion), ctx);
                gtk_table_attach(GTK_TABLE(ctx->table), ctx->bt_cellules, 0, 9, 0, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_set_sensitive(ctx->bt_cellules, FALSE);
                gtk_widget_show(ctx->bt_cellules);

                // 9*9 boutons pour les cases
                for(j=0; j<9; j++) {
                    for(i=0; i<9; i++) {
                        ctx->bts_cases[i][j] = gtk_button_new ();
                        gtk_table_attach(GTK_TABLE(ctx->table), ctx->bts_cases[i][j], i, i+1, j+2, j+3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                        g_signal_connect(G_OBJECT(ctx->bts_cases[i][j]), "clicked", G_CALLBACK(action_pion), ctx);
                        gtk_widget_set_sensitive(ctx->bts_cases[i][j], FALSE);
                        gtk_widget_show(ctx->bts_cases[i][j]);
                    }
                }

                // label & score joueur 1
                ctx->lbl_J1 = gtk_label_new("Joueur 1");
                gtk_table_attach(GTK_TABLE(ctx->table), ctx->lbl_J1, 0, 2, 11, 12, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(ctx->lbl_J1);

                ctx->score_J1 = gtk_entry_new();
                gtk_entry_set_editable(GTK_ENTRY(ctx->score_J1), FALSE);
                gtk_entry_set_width_chars(GTK_ENTRY(ctx->score_J1), 2);
                gtk_entry_set_alignment(GTK_ENTRY(ctx->score_J1), 0.5);
                gtk_table_attach(GTK_TABLE(ctx->table), ctx->score_J1, 0, 2, 12, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(ctx->score_J1);

                // bouton sortie
                ctx->bt_sortie = gtk_button_new_with_label("Sortie");
                g_signal_connect(G_OBJECT(ctx->bt_sortie), "clicked", G_CALLBACK(action_pion), ctx);
                gtk_table_attach(GTK_TABLE(ctx->table), ctx->bt_sortie, 2, 7, 11, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_set_sensitive(ctx->bt_sortie, FALSE);
                gtk_widget_show(ctx->bt_sortie);

                // label joueur 2
                ctx->lbl_J2 = gtk_label_new("Joueur 2");
                gtk_table_attach(GTK_TABLE(ctx->table), ctx->lbl_J2, 7, 9, 11, 12, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(ctx->lbl_J2);

                ctx->score_J2 = gtk_entry_new();
                gtk_entry_set_editable(GTK_ENTRY(ctx->score_J2), FALSE);
                gtk_entry_set_width_chars(GTK_ENTRY(ctx->score_J2), 2);
                gtk_entry_set_alignment(GTK_ENTRY(ctx->score_J2), 0.5);
                gtk_table_attach(GTK_TABLE(ctx->table), ctx->score_J2, 7, 9, 12, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(ctx->score_J2);

            gtk_widget_show(ctx->table);

        gtk_widget_show(ctx->aframe);

        // label statut
        ctx->lbl_statut = gtk_label_new("Bonjour !");
        gtk_box_pack_start(GTK_BOX(ctx->vbox), ctx->lbl_statut, FALSE, FALSE, 0);
        gtk_widget_show(ctx->lbl_statut);

    gtk_widget_show(ctx->vbox);

    return ctx->vbox;
}

