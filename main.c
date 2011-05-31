#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joueur.h"
#include "evasion.h"

typedef struct {
    GtkWidget *fenetre;
        GtkWidget *vbox;
            GtkWidget *menu;
            GtkWidget *aframe;
                GtkWidget *table;
                    GtkWidget *bt_cellules;
                    GtkWidget *bts_cases[9][9];
                    GtkWidget *lbl_J1;
                    GtkWidget *bt_sortie;
                    GtkWidget *lbl_J2;
            GtkWidget *lbl_statut;

    JE_jeu jeu;
} JE_contexte;

void maj_appar_bouton(GtkWidget *bt, case_t c) {
    GdkColor clr = {0};

    if(c&CASE_GRISE) {  // gris 50%
        clr.red = 32768;
        clr.green = 32768;
        clr.blue = 32768;
    }

    if(c&CASE_PEUTCLIQUER) {    // vert
        clr.red = 0;
        clr.green = 65535;
        clr.blue = 0;
        gtk_widget_set_sensitive(bt, TRUE);
    }
    else
        gtk_widget_set_sensitive(bt, FALSE);

    gtk_widget_modify_bg(bt, GTK_STATE_NORMAL, &clr);
    gtk_widget_modify_bg(bt, GTK_STATE_INSENSITIVE, &clr);
}

void maj_etat(JE_contexte *ctx) {
    char buffer[32];
    int i, j;

    // mise à jour du label d'état
    etat_t e = JE_etat(&ctx->jeu);

    switch(ETAT_ETAT(e)) {
    case ETAT_J1:
        strcpy(buffer, "Joueur 1");
        break;
    case ETAT_J2:
        strcpy(buffer, "Joueur 2");
        break;
    case ETAT_J1WIN:
        strcpy(buffer, "Joueur 1 gagne");
        break;
    case ETAT_J2WIN:
        strcpy(buffer, "Joueur 2 gagne");
        break;
    default:
        sprintf(buffer, "Etat %d", e);
        break;
    }

    gtk_label_set_text(GTK_LABEL(ctx->lbl_statut), buffer);

    // mise à jour du plateau
    maj_appar_bouton(ctx->bt_cellules, ctx->jeu.part[0]);

    for(j=0; j<9; j++)
        for(i=0; i<9; i++) {
            case_t c = ctx->jeu.tab[i][j];
            char *txt = NULL;
            GtkWidget *bt = ctx->bts_cases[i][j];

            switch(CASE_TYPE(c)) {
            case CASE_LIBRE:
                txt = " ";
                break;
            case CASE_J1:
                txt = "1";
                break;
            case CASE_J2:
                txt = "2";
                break;
            case CASE_GARDIEN:
                txt = "G";
                break;
            default:
                break;
            }

            gtk_button_set_label(GTK_BUTTON(bt), txt);

            maj_appar_bouton(bt, c);
        }

    maj_appar_bouton(ctx->bt_sortie, ctx->jeu.part[1]);
}

void nouvelle_partie(GtkWidget *w, JE_contexte *ctx) {
    JE_nouvellepartie(&ctx->jeu);

    maj_etat(ctx);
}

void sauver_partie(GtkWidget *w, JE_contexte *ctx) {
    // TODO
}

void charger_partie(GtkWidget *w, JE_contexte *ctx) {
    // TODO
}

void quitter_partie(GtkWidget *w, JE_contexte *ctx) {
    gtk_main_quit();
}

void action_pion(GtkWidget *widget, JE_contexte *ctx) {
    int i, j, x, y;

    // on cherche le bouton qui a déclenché ça (:s)
    if(widget == ctx->bt_cellules)
        y = -1;
    else if(widget == ctx->bt_sortie)
        y = 9;
    else
        for(j=0; j<9; j++)
            for(i=0; i<9; i++)
                if(widget == ctx->bts_cases[i][j]) {
                    x = i;
                    y = j;
                }

printf("bouton%d,%d appuyé\n", x, y);

    // on fait l'action demandée en fonction de si on vient de choisir le pion ou sa destination
    if(JE_etat(&ctx->jeu)&ETAT_ATTENTEBOUGER)
        JE_bougerpion(&ctx->jeu, x, y);
    else
        JE_selectpion(&ctx->jeu, x, y);

    // et on remet à jour l'IHM pour refléter les modifs
    maj_etat(ctx);
}

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

int main(int argc,char *argv[]) {
    GError *error;
    JE_contexte ev;
    GdkGeometry hints;
    int i;
    int j;

    bzero(&ev, sizeof(ev));
    gtk_init(&argc,&argv);

    // création fenêtre
    ev.fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ev.fenetre), "Jeu d'évasion");
    g_signal_connect(G_OBJECT(ev.fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

        // vbox principale
        ev.vbox = gtk_vbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(ev.fenetre), ev.vbox);

            // menu
            GtkActionGroup *action_group;
            GtkUIManager *menu_manager;
            action_group = gtk_action_group_new("MenuActions");
            gtk_action_group_add_actions(action_group, menu_entries, G_N_ELEMENTS(menu_entries), &ev);
            menu_manager = gtk_ui_manager_new();
            gtk_ui_manager_insert_action_group(menu_manager, action_group, 0);
            error = NULL;
            gtk_ui_manager_add_ui_from_file(menu_manager, "menu.ui", &error);
            if(error) {
                g_message("building menus failed: %s", error->message);
                g_error_free(error);
            }
            gtk_window_add_accel_group(GTK_WINDOW(ev.fenetre), gtk_ui_manager_get_accel_group(menu_manager));
            ev.menu = gtk_ui_manager_get_widget(menu_manager, "/MainMenu");
            gtk_box_pack_start(GTK_BOX(ev.vbox), ev.menu, FALSE, FALSE, 0);
            gtk_widget_show(ev.menu);

            // aspect frame (frame de ratio constant pour être sûr que les cases restent carrées)
            ev.aframe = gtk_aspect_frame_new(NULL, 0.5, 0.5, 9.0/13.0, 0);
            gtk_box_pack_start(GTK_BOX(ev.vbox), ev.aframe, TRUE, TRUE, 0);

                // la table qui contient le plateau de jeu
                ev.table = gtk_table_new(13, 9, TRUE);
                gtk_container_add(GTK_CONTAINER(ev.aframe), ev.table);

                    // cellules
                    ev.bt_cellules = gtk_button_new_with_label("Cellules");
                    g_signal_connect(G_OBJECT(ev.bt_cellules), "clicked", G_CALLBACK(action_pion), &ev);
                    gtk_table_attach(GTK_TABLE(ev.table), ev.bt_cellules, 0, 9, 0, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                    gtk_widget_show(ev.bt_cellules);

                    // 9*9 boutons pour les cases
                    for(j=0; j<9; j++) {
                        for(i=0; i<9; i++) {
                            ev.bts_cases[i][j] = gtk_button_new ();
                            gtk_table_attach(GTK_TABLE(ev.table), ev.bts_cases[i][j], i, i+1, j+2, j+3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                            g_signal_connect(G_OBJECT(ev.bts_cases[i][j]), "clicked", G_CALLBACK(action_pion), &ev);
                            gtk_widget_show(ev.bts_cases[i][j]);
                        }
                    }

                    // label joueur 1
                    ev.lbl_J1 = gtk_label_new("Joueur 1");
                    gtk_table_attach(GTK_TABLE(ev.table), ev.lbl_J1, 0, 2, 11, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                    gtk_widget_show(ev.lbl_J1);

                    // bouton sortie
                    ev.bt_sortie = gtk_button_new_with_label("Sortie");
                    g_signal_connect(G_OBJECT(ev.bt_sortie), "clicked", G_CALLBACK(action_pion), &ev);
                    gtk_table_attach(GTK_TABLE(ev.table), ev.bt_sortie, 2, 7, 11, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                    gtk_widget_show(ev.bt_sortie);

                    // label joueur 2
                    ev.lbl_J2 = gtk_label_new("Joueur 2");
                    gtk_table_attach(GTK_TABLE(ev.table), ev.lbl_J2, 7, 9, 11, 13, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                    gtk_widget_show(ev.lbl_J2);

                gtk_widget_show(ev.table);

            gtk_widget_show(ev.aframe);

            // label statut
            ev.lbl_statut = gtk_label_new("Bonjour !");
            gtk_box_pack_start(GTK_BOX(ev.vbox), ev.lbl_statut, FALSE, FALSE, 0);
            gtk_widget_show(ev.lbl_statut);

        gtk_widget_show(ev.vbox);

    hints.min_aspect = 9.0/(13.0+2.0);
    hints.max_aspect = 9.0/(13.0+2.0);
    gtk_window_set_geometry_hints(GTK_WINDOW(ev.fenetre), NULL, &hints, GDK_HINT_ASPECT);
    gtk_widget_show(ev.fenetre);

    // let's go !
    gtk_main();

    return 0;
}

