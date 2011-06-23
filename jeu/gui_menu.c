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
    // on charge la partie
    io_charger(GTK_WINDOW(ctx->gui.fenetre), &ctx->filename, &ctx->jeu, &ctx->J1, &ctx->J2);

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

