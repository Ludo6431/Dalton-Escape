#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commun/joueur.h"
#include "commun/io.h"
#include "libevasion/evasion.h"
#include "jeu/gui_action.h"
#include "jeu/jeu.h"

#include "jeu/gui_menu.h"

void nouvelle_partie(GtkWidget *w, JEU *ctx) {
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

    // on réinitialise qques variables dans le cas où recommence une partie
    ctx->jeu.joueur_debut = 0;
    ctx->J1.score = 0;
    ctx->J2.score = 0;

    ev_nouvellepartie(&ctx->jeu);

    // on cache l'image de bienvenue et on affiche la table de jeu
    gtk_widget_hide(ctx->gui.bienvenue);
    gtk_widget_show(ctx->gui.aframe);

    maj_etat(ctx);
}

void afficher_scores(GtkWidget *w, JEU *ctx) {
    GtkWidget *dialog, *content, *label;
    char buffer[256];

    dialog = gtk_dialog_new_with_buttons("Bravo !", GTK_WINDOW(ctx->gui.fenetre), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_STOCK_OK, 1,
        NULL
    );
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        sprintf(buffer, "Scores :\n%s: %d\n%s: %d\n",
            ctx->J1.pseudo, ctx->J1.score, ctx->J2.pseudo, ctx->J2.score
        );
        label = gtk_label_new(buffer);
        gtk_box_pack_start(GTK_BOX(content), label, TRUE, TRUE, 0);
        gtk_widget_show(label);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void sauvegarder_partie(GtkWidget *w, JEU *ctx) {
    // on sauvegarde la partie
    io_sauvegarder(GTK_WINDOW(ctx->gui.fenetre), &ctx->filename, &ctx->jeu, &ctx->J1, &ctx->J2);
}

void charger_partie(GtkWidget *w, JEU *ctx) {
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
    jeu_vide_coups(ctx);
    ctx->jeu.etat |= ETAT_ENREGCOUP;

    // on cache l'image de bienvenue et on affiche la table de jeu
    gtk_widget_hide(ctx->gui.bienvenue);
    gtk_widget_show(ctx->gui.aframe);

    // et on met à jour l'IHM
    maj_etat(ctx);
}

void quitter_partie(GtkWidget *w, JEU *ctx) {
    gtk_main_quit();
}


void annuler_coup(GtkWidget *w, JEU *ctx) {
    jeu_annule_coup(ctx);

    maj_etat(ctx);
}

void refaire_coup(GtkWidget *w, JEU *ctx) {
    jeu_refait_coup(ctx);

    maj_etat(ctx);
}

