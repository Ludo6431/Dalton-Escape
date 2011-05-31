#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joueur.h"
#include "evasion.h"

typedef struct {
    GtkWidget *fenetre;
        GtkWidget *vbox;
            GtkWidget *hbox_menu;
                GtkWidget *bt_nouvelle_partie;
                GtkWidget *bt_quitter;
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

void maj_etat_bouton(GtkWidget *bt, case_t c) {
    GdkColor clr = {0};

    if(c&CASE_GRISE) {
        clr.red = 32768;
        clr.green = 32768;
        clr.blue = 32768;
    }

    if(c&CASE_PEUTCLIQUER) {
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
    maj_etat_bouton(ctx->bt_cellules, ctx->jeu.part[0]);

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

            maj_etat_bouton(bt, c);
        }

    maj_etat_bouton(ctx->bt_sortie, ctx->jeu.part[1]);
}

void nouvelle_partie(GtkWidget *widget, JE_contexte *ctx) {
    JE_nouvellepartie(&ctx->jeu);

    maj_etat(ctx);
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

    if(JE_etat(&ctx->jeu)&ETAT_ATTENTEBOUGER)
        JE_bougerpion(&ctx->jeu, x, y);
    else
        JE_selectpion(&ctx->jeu, x, y);

    maj_etat(ctx);
}

int main(int argc,char *argv[]) {
    JE_contexte ev;
    GdkGeometry hints;
    int i;
    int j;

    bzero(&ev, sizeof(ev));
    gtk_init(&argc,&argv);

    // création fenêtre
    ev.fenetre=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ev.fenetre),"jeu d'évasion");
    g_signal_connect(G_OBJECT(ev.fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

        // vbox principale
        ev.vbox = gtk_vbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(ev.fenetre), ev.vbox);

            // hbox du menu
            ev.hbox_menu = gtk_hbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(ev.vbox), ev.hbox_menu, FALSE, FALSE, 0);

                // bouton nouvelle partie
                ev.bt_nouvelle_partie = gtk_button_new_with_label("nouvelle partie");
                g_signal_connect(G_OBJECT(ev.bt_nouvelle_partie), "clicked", G_CALLBACK(nouvelle_partie), &ev);
                gtk_box_pack_start(GTK_BOX(ev.hbox_menu), ev.bt_nouvelle_partie, FALSE, FALSE, 0);
                gtk_widget_show(ev.bt_nouvelle_partie);

                // bouton quitter
                ev.bt_quitter = gtk_button_new_with_label("Quitter");
                g_signal_connect(G_OBJECT(ev.bt_quitter), "clicked", G_CALLBACK(gtk_main_quit), NULL);
                gtk_box_pack_end(GTK_BOX(ev.hbox_menu), ev.bt_quitter, FALSE, FALSE, 0);
                gtk_widget_show(ev.bt_quitter);

            gtk_widget_show(ev.hbox_menu);

            // aspect frame
            ev.aframe = gtk_aspect_frame_new(NULL, 0.5, 0.5, 9.0/13.0, 0);
            gtk_box_pack_start(GTK_BOX(ev.vbox), ev.aframe, TRUE, TRUE, 0);

                // frame
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

