#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commun/joueur.h"
#include "commun/io.h"
#include "libevasion/evasion.h"
#include "editeur/gui_action.h"

#include "editeur/gui_menu.h"

void nouvelle_partie(GtkWidget *w, EDIT *ctx) {
    GdkColor color;

    gdk_color_parse("yellow", &color);
    joueur_init(&ctx->J1, "Nouveau joueur", "Joueur 1", &color, GTK_WINDOW(ctx->gui.fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->gui.lbl_J1), ctx->J1.pseudo);

    gdk_color_parse("red", &color);
    joueur_init(&ctx->J2, "Nouveau joueur", "Joueur 2", &color, GTK_WINDOW(ctx->gui.fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->gui.lbl_J2), ctx->J2.pseudo);

    //
    if(gdk_color_equal(&ctx->J1.color, &ctx->J2.color)) {
        ctx->J2.color.red = 65535-ctx->J1.color.red;
        ctx->J2.color.green = 65535-ctx->J1.color.green;
        ctx->J2.color.blue = 65535-ctx->J1.color.blue;
    }

    ev_nouvellepartie(&ctx->jeu);

    // on cache l'image de bienvenue et on affiche la table de jeu
    gtk_widget_hide(ctx->gui.bienvenue);
    gtk_widget_show(ctx->gui.aframe);

    // on met à jour le nb de pions sortis
    char buffer[32];
    sprintf(buffer, "%d", ctx->jeu.nb_p_sort[0]);
    gtk_entry_set_text(GTK_ENTRY(ctx->gui.score_J1), buffer);
    sprintf(buffer, "%d", ctx->jeu.nb_p_sort[1]);
    gtk_entry_set_text(GTK_ENTRY(ctx->gui.score_J2), buffer);

    maj_etat(ctx);
}

void sauvegarder_partie(GtkWidget *w, EDIT *ctx) {
    EV jeu;
    int ret;
    memcpy(&jeu, &ctx->jeu, sizeof(EV));

    // on récupère les nb_p_sort que l'utilisateur peut avoir modifié
    {
        char buffer[32];
        int score;

        strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(ctx->gui.score_J1)));
        if(sscanf(buffer, "%d", &score)==1) {
            jeu.nb_p_cell[0] -= score - jeu.nb_p_sort[0];
            jeu.nb_p_sort[0] = score;
        }
        strcpy(buffer, gtk_entry_get_text(GTK_ENTRY(ctx->gui.score_J2)));
        if(sscanf(buffer, "%d", &score)==1) {
            jeu.nb_p_cell[1] -= score - jeu.nb_p_sort[1];
            jeu.nb_p_sort[1] = score;
        }
    }

    // on demande quel joueur commencera la partie
    {
        GtkWidget *dialog, *content, *label;
        int resp;

        dialog = gtk_dialog_new_with_buttons("Joueur qui commence", GTK_WINDOW(ctx->gui.fenetre), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            ctx->J1.pseudo, ETAT_J1,
            ctx->J2.pseudo, ETAT_J2,
            NULL
        );
        content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            label = gtk_label_new("Choisir le joueur qui commencera à jouer");
            gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 0);
            gtk_widget_show(label);

        resp = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        assert(resp==ETAT_J1 || resp==ETAT_J2);

        jeu.etat &= ~ETAT_MASQETAT;
        jeu.etat |= resp;

        jeu.joueur_debut = !ETAT2JOUE(jeu.etat);
    }

    // on fait attention à ce que la partie sauvegardée soit valide
    ret = ev_verifie(&jeu, 0, 1);   // est-ce que le contexte est bon ?
    if(ret) {   // non! on essaye de le réparer ?
        GtkWidget *dialog, *content, *label;
        int resp;

        dialog = gtk_dialog_new_with_buttons("Impossible de sauvegarder", GTK_WINDOW(ctx->gui.fenetre), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
            NULL
        );
        content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            label = gtk_label_new("Il est impossible de sauvegarder en l'état.\nVoulez-vous sauvegarder en rectifiant le fichier ou annuler ?");
            gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 0);
            gtk_widget_show(label);

        resp = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if(resp == GTK_RESPONSE_REJECT)
            return;
    }
    ret = ev_verifie(&jeu, 1, 1);   // on essaye de réparer le contexte
    if(ret) {   // le contexte n'est pas réparable
        GtkWidget *dialog, *content, *label;

        dialog = gtk_dialog_new_with_buttons("Impossible de sauvegarder", GTK_WINDOW(ctx->gui.fenetre), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
            NULL
        );
        content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            label = gtk_label_new("Il est impossible de sauvegarder en rectifiant l'état.");
            gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 0);
            gtk_widget_show(label);

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        return;
    }

    // on sauvegarde la partie
    io_sauvegarder(GTK_WINDOW(ctx->gui.fenetre), &ctx->filename, &jeu, &ctx->J1, &ctx->J2);
}

void charger_partie(GtkWidget *w, EDIT *ctx) {
    EV jeu;
    JOUEUR J1, J2;
    int ret;

    // on charge la partie
    io_charger(GTK_WINDOW(ctx->gui.fenetre), &ctx->filename, &jeu, &J1, &J2);

    // on fait attention à ce que la partie sauvegardée soit valide
    ret = ev_verifie(&jeu, 0, 1);   // est-ce que le contexte est bon ?
    if(ret) {   // non! on essaye de le réparer ?
        GtkWidget *dialog, *content, *label;
        int resp;

        dialog = gtk_dialog_new_with_buttons("Impossible de charger", GTK_WINDOW(ctx->gui.fenetre), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
            NULL
        );
        content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            label = gtk_label_new("Il est impossible de charger en l'état.\nVoulez-vous charger en rectifiant le fichier ou annuler ?");
            gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 0);
            gtk_widget_show(label);

        resp = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if(resp == GTK_RESPONSE_REJECT)
            return;
    }
    ret = ev_verifie(&jeu, 1, 1);   // on essaye de réparer le contexte
    if(ret) {   // le contexte n'est pas réparable
        GtkWidget *dialog, *content, *label;

        dialog = gtk_dialog_new_with_buttons("Impossible de charger", GTK_WINDOW(ctx->gui.fenetre), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
            NULL
        );
        content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            label = gtk_label_new("Il est impossible de charger en rectifiant l'état.");
            gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 0);
            gtk_widget_show(label);

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        return;
    }

    //
    if(gdk_color_equal(&J1.color, &J2.color)) {
        J2.color.red = 65535-J1.color.red;
        J2.color.green = 65535-J1.color.green;
        J2.color.blue = 65535-J1.color.blue;
    }

    // tout est ok, on charge
    memcpy(&ctx->jeu, &jeu, sizeof(EV));
    memcpy(&ctx->J1, &J1, sizeof(JOUEUR));
    memcpy(&ctx->J2, &J2, sizeof(JOUEUR));

    // on réinitialise la liste des coups enregistrés et on empile l'état actuel
    editeur_vide_coups(ctx);
    ctx->jeu.etat |= ETAT_ENREGCOUP;

    // on cache l'image de bienvenue et on affiche la table de jeu
    gtk_widget_hide(ctx->gui.bienvenue);
    gtk_widget_show(ctx->gui.aframe);

    // on met à jour le nb de pions sortis
    char buffer[32];
    sprintf(buffer, "%d", ctx->jeu.nb_p_sort[0]);
    gtk_entry_set_text(GTK_ENTRY(ctx->gui.score_J1), buffer);
    sprintf(buffer, "%d", ctx->jeu.nb_p_sort[1]);
    gtk_entry_set_text(GTK_ENTRY(ctx->gui.score_J2), buffer);

    // et on met à jour l'IHM
    maj_etat(ctx);
}

void quitter_partie(GtkWidget *w, EDIT *ctx) {
    gtk_main_quit();
}

void afficher_apropos(GtkWidget *w, EDIT *ctx) {
    GtkWidget *dialog = gtk_about_dialog_new();

    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Dalton Escape Editeur");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.1"); 
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "(c) Anne-Claire Poujol De Molliens & Ludovic Lacoste");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "L'éditeur d'un jeu d'évasion.");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://github.com/Ludo6431/Dalton-Escape");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


void annuler_coup(GtkWidget *w, EDIT *ctx) {
    editeur_annule_coup(ctx);

    maj_etat(ctx);
}

void refaire_coup(GtkWidget *w, EDIT *ctx) {
    editeur_refait_coup(ctx);

    maj_etat(ctx);
}

