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
    joueur_init(&ctx->J1, "Nouveau joueur", "Joueur 1", GTK_WINDOW(ctx->gui.fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->gui.lbl_J1), ctx->J1.pseudo);
    joueur_init(&ctx->J2, "Nouveau joueur", "Joueur 2", GTK_WINDOW(ctx->gui.fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->gui.lbl_J2), ctx->J2.pseudo);

    ev_nouvellepartie(&ctx->jeu);

    maj_etat(ctx);
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

    // tout est ok, on charge
    memcpy(&ctx->jeu, &jeu, sizeof(EV));
    memcpy(&ctx->J1, &J1, sizeof(JOUEUR));
    memcpy(&ctx->J2, &J2, sizeof(JOUEUR));

//TODO: vider liste anciens coups    ctx->jeu.etat |= ETAT_ENREGCOUP;

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

