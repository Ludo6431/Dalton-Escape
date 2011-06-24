#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editeur/gui_menu.h"
#include "editeur/gui_action.h"

#include "editeur/gui.h"

static inline void _init_case(GtkWidget *bt, void *user_ptr) {
    // pour le mode de déplacement: sélection source puis sélection départ
    g_signal_connect(G_OBJECT(bt), "clicked", G_CALLBACK(action_pion), user_ptr);

    // pour le mode de déplacement: glisser-déposer (aka: drag&drop)
    g_signal_connect(G_OBJECT(bt), "drag-begin", G_CALLBACK(debut_drag), user_ptr);                 // source
    g_signal_connect(G_OBJECT(bt), "drag-drop", G_CALLBACK(drop_drag), user_ptr);                   // destination
    g_signal_connect(G_OBJECT(bt), "drag-data-get", G_CALLBACK(demande_donnee_drag), user_ptr);     // source
    g_signal_connect(G_OBJECT(bt), "drag-data-received", G_CALLBACK(recoit_donnee_drag), user_ptr); // destination
    g_signal_connect(G_OBJECT(bt), "drag-end", G_CALLBACK(fin_drag), user_ptr);                     // source

    gtk_widget_set_sensitive(bt, FALSE);
}

static GtkActionEntry menu_entries[] = {
    /* name, stock id, label, accelerator, tooltip, callback */
    {"GameMenuAction", NULL, "_Jeu", "<control>J"},
    {"NewAction", GTK_STOCK_NEW, "Nouveau", "<control>N", "Commence une nouvelle partie", G_CALLBACK(nouvelle_partie)},
    {"SaveAction", GTK_STOCK_SAVE_AS, "Sauvegarder", "<control>S", "Sauvegarder la partie en cours", G_CALLBACK(sauvegarder_partie)},
    {"LoadAction", GTK_STOCK_OPEN, "Charger", "<control>O", "Charger une partie depuis un fichier", G_CALLBACK(charger_partie)},
    {"QuitAction", GTK_STOCK_QUIT, "Quitter", "<control>Q", "Quitter le jeu", G_CALLBACK(quitter_partie)},

    {"EditMenuAction", NULL, "E_diter", "<control>D"},
    {"UndoAction", GTK_STOCK_UNDO, "Annuler", "<control>Z", "Annule le dernier coup", G_CALLBACK(annuler_coup)},
    {"RedoAction", GTK_STOCK_REDO, "Refaire", "<shift><control>Z", "Refait le dernier coup annulé", G_CALLBACK(refaire_coup)},

    {"HelpMenuAction", NULL, "Aid_e", "<control>E"},
    {"AboutAction", GTK_STOCK_ABOUT, "A propos", NULL, "A propos du jeu", G_CALLBACK(afficher_apropos)},
};

// crée l'interface que l'on peut ajouter dans n'importe quel container
GtkWidget *gui_init(GUI *gui, GtkWindow *fenpar, void *user_ptr) {
    int i;
    int j;

    // création du style pour les différents boutons
    GtkStockItem items[] = {
        { "case-J1", "", 0, 0, "" },
        { "case-J2", "", 0, 0, "" },
        { "case-gardien", "", 0, 0, "" }
    };
    gtk_stock_add(items, G_N_ELEMENTS(items));

    // on garde au chaud la fenêtre où on est
    gui->fenetre = GTK_WIDGET(fenpar);

    // vbox principale
    gui->vbox = gtk_vbox_new(FALSE, 0);

        // menu
        {
            GError *error;
            GtkActionGroup *action_group;

            action_group = gtk_action_group_new("MenuActions");
            gtk_action_group_add_actions(action_group, menu_entries, G_N_ELEMENTS(menu_entries), user_ptr);
            gui->menu_manager = gtk_ui_manager_new();
            gtk_ui_manager_insert_action_group(gui->menu_manager, action_group, 0);
            error = NULL;
            gtk_ui_manager_add_ui_from_file(gui->menu_manager, "editeur_menu.ui", &error);
            if(error) {
                g_message("building menus failed: %s", error->message);
                g_error_free(error);
            }
            gtk_window_add_accel_group(GTK_WINDOW(gui->fenetre), gtk_ui_manager_get_accel_group(gui->menu_manager));

            gtk_widget_set_sensitive(gtk_ui_manager_get_widget(gui->menu_manager, "/MainMenu/EditMenu/Undo"), FALSE);
            gtk_widget_set_sensitive(gtk_ui_manager_get_widget(gui->menu_manager, "/MainMenu/EditMenu/Redo"), FALSE);
        }
        gui->menu = gtk_ui_manager_get_widget(gui->menu_manager, "/MainMenu");
        gtk_box_pack_start(GTK_BOX(gui->vbox), gui->menu, FALSE, FALSE, 0);
        gtk_widget_show(gui->menu);

        // image de bienvenue
        gui->bienvenue = gtk_image_new_from_file("bienvenue.png");
        gtk_box_pack_start(GTK_BOX(gui->vbox), gui->bienvenue, TRUE, TRUE, 0);
        gtk_widget_show(gui->bienvenue);

        // aspect frame (frame de ratio constant pour être sûr que les cases restent carrées)
        gui->aframe = gtk_aspect_frame_new(NULL, 0.5, 0.5, 9.0/13.0, 0);
        gtk_box_pack_start(GTK_BOX(gui->vbox), gui->aframe, TRUE, TRUE, 0);

            // la table qui contient le plateau de jeu
            gui->table = gtk_table_new(13, 9, TRUE);
            gtk_container_add(GTK_CONTAINER(gui->aframe), gui->table);

                // cellules J1
                gui->bt_cellules[0] = gtk_button_new_with_label("Cellules J1");
                gtk_table_attach(GTK_TABLE(gui->table), gui->bt_cellules[0], 0, 4, 0, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                _init_case(gui->bt_cellules[0], user_ptr);
                gtk_widget_show(gui->bt_cellules[0]);

                // cellules J2
                gui->bt_cellules[1] = gtk_button_new_with_label("Cellules J2");
                gtk_table_attach(GTK_TABLE(gui->table), gui->bt_cellules[1], 5, 9, 0, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                _init_case(gui->bt_cellules[1], user_ptr);
                gtk_widget_show(gui->bt_cellules[1]);

                // 9*9 boutons pour les cases
                for(j=0; j<9; j++) {
                    for(i=0; i<9; i++) {
                        gui->bts_cases[i][j] = gtk_button_new ();
                        gtk_table_attach(GTK_TABLE(gui->table), gui->bts_cases[i][j], i, i+1, j+2, j+3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                        _init_case(gui->bts_cases[i][j], user_ptr);
                        gtk_widget_show(gui->bts_cases[i][j]);
                    }
                }

                // label & score joueur 1
                gui->lbl_J1 = gtk_label_new("Joueur 1");
                gtk_table_attach(GTK_TABLE(gui->table), gui->lbl_J1, 0, 2, 11, 12, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(gui->lbl_J1);

                gui->score_J1 = gtk_entry_new();
                gtk_entry_set_editable(GTK_ENTRY(gui->score_J1), TRUE);
                gtk_entry_set_width_chars(GTK_ENTRY(gui->score_J1), 2);
                gtk_entry_set_alignment(GTK_ENTRY(gui->score_J1), 0.5);
                gtk_table_attach(GTK_TABLE(gui->table), gui->score_J1, 0, 2, 12, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(gui->score_J1);

                // bouton sortie
                gui->bt_sortie = gtk_button_new_with_label("Sortie");
                gtk_table_attach(GTK_TABLE(gui->table), gui->bt_sortie, 2, 7, 11, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                _init_case(gui->bt_sortie, user_ptr);
                gtk_widget_show(gui->bt_sortie);

                // label joueur 2
                gui->lbl_J2 = gtk_label_new("Joueur 2");
                gtk_table_attach(GTK_TABLE(gui->table), gui->lbl_J2, 7, 9, 11, 12, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(gui->lbl_J2);

                gui->score_J2 = gtk_entry_new();
                gtk_entry_set_editable(GTK_ENTRY(gui->score_J2), TRUE);
                gtk_entry_set_width_chars(GTK_ENTRY(gui->score_J2), 2);
                gtk_entry_set_alignment(GTK_ENTRY(gui->score_J2), 0.5);
                gtk_table_attach(GTK_TABLE(gui->table), gui->score_J2, 7, 9, 12, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(gui->score_J2);

            gtk_widget_show(gui->table);

        gtk_widget_hide(gui->aframe);   // on la cache au début pour afficher une image

        // label statut
        gui->lbl_statut = gtk_label_new("Bonjour !");
        gtk_box_pack_start(GTK_BOX(gui->vbox), gui->lbl_statut, FALSE, FALSE, 0);
        gtk_widget_show(gui->lbl_statut);

    return gui->vbox;
}

