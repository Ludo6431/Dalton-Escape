#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JE_joueur.h"
#include "JE_jeu.h"

#include "JE_callbacks.h"

void maj_appar_bouton(GtkWidget *bt, case_t c) {
    GtkWidget *image = NULL;

    if(c&CASE_GRISE)
        gtk_widget_set_name(bt, "case_grise");
    else
        gtk_widget_set_name(bt, "case");

    if(c&CASE_PEUTCLIQUER)
        gtk_widget_set_sensitive(bt, TRUE);
    else
        gtk_widget_set_sensitive(bt, FALSE);

    switch(CASE_TYPE(c)) {
    case CASE_J1:
        image = gtk_image_new_from_stock("case-J1", GTK_ICON_SIZE_DND);
        break;
    case CASE_J2:
        image = gtk_image_new_from_stock("case-J2", GTK_ICON_SIZE_DND);
        break;
    case CASE_GARDIEN:
        image = gtk_image_new_from_stock("case-gardien", GTK_ICON_SIZE_DND);
        break;
    default:
        break;
    }

    gtk_button_set_image(GTK_BUTTON(bt), image);
}

void maj_etat(JE *ctx) {
    char buffer[32];
    int i, j;

    // mise à jour du label d'état
    etat_t e = jeu_etat(&ctx->jeu);

    switch(ETAT_ETAT(e)) {
    case ETAT_J1:
        sprintf(buffer, "Au tour de %s", ctx->J1.pseudo);
        break;
    case ETAT_J2:
        sprintf(buffer, "Au tour de %s", ctx->J2.pseudo);
        break;
    case ETAT_J1WIN:
        sprintf(buffer, "%s gagne", ctx->J1.pseudo);
        ctx->J1.score++;
        break;
    case ETAT_J2WIN:
        sprintf(buffer, "%s gagne", ctx->J2.pseudo);
        ctx->J2.score++;
        break;
    default:
        sprintf(buffer, "Autre état : %d", e);
        break;
    }

    gtk_label_set_text(GTK_LABEL(ctx->lbl_statut), buffer);

    // mise à jour des labels de scores
    sprintf(buffer, "%d", ctx->J1.score);
    gtk_entry_set_text(GTK_ENTRY(ctx->score_J1), buffer);
    sprintf(buffer, "%d", ctx->J2.score);
    gtk_entry_set_text(GTK_ENTRY(ctx->score_J2), buffer);

    // mise à jour du plateau
    maj_appar_bouton(ctx->bt_cellules, ctx->jeu.part[0]);

    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            maj_appar_bouton(ctx->bts_cases[i][j], ctx->jeu.tab[i][j]);

    maj_appar_bouton(ctx->bt_sortie, ctx->jeu.part[1]);
}

void nouvelle_partie(GtkWidget *w, JE *ctx) {
    joueur_init(&ctx->J1, "Nouveau joueur", "Joueur 1", GTK_WINDOW(ctx->fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->lbl_J1), ctx->J1.pseudo);
    joueur_init(&ctx->J2, "Nouveau joueur", "Joueur 2", GTK_WINDOW(ctx->fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->lbl_J2), ctx->J2.pseudo);

    jeu_nouvellepartie(&ctx->jeu);

    maj_etat(ctx);
}

#define EXT_FICHIER ".esc"

void sauver_partie(GtkWidget *w, JE *ctx) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Sauvegarder partie", GTK_WINDOW(ctx->fenetre), GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*"EXT_FICHIER);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    if(!ctx->filename) {    // on n'a pas chargé de partie
//        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "~/");
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "partie"EXT_FICHIER);
    }
    else
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), ctx->filename);

    if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if(ctx->filename) free(ctx->filename);
        ctx->filename = strdup(filename);

        FILE *fd = fopen(filename, "wb+");
        g_free (filename);
        if(!fd)
            printf("wtf!!\n");

        // TODO: add header ?

        // ordre important
        jeu_sauverpartie(&ctx->jeu, fd);
        joueur_sauver(&ctx->J1, fd);
        joueur_sauver(&ctx->J2, fd);

        // TODO: append checksum ?

        fclose(fd);
    }
    gtk_widget_destroy (dialog);
}

void charger_partie(GtkWidget *w, JE *ctx) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Ouvrir partie", GTK_WINDOW(ctx->fenetre), GTK_FILE_CHOOSER_ACTION_OPEN,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*"EXT_FICHIER);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        ctx->filename = strdup(filename);   // on garde une copie pour être au bon endroit au moment de sauvegarder

        FILE *fd = fopen(filename, "rb+");
        g_free(filename);
        if(!fd)
            printf("wtf!?\n");

        // TODO: verify checksum (and header ?)

        // ordre important
        jeu_chargerpartie(&ctx->jeu, fd);
        joueur_charger(&ctx->J1, fd);
        joueur_charger(&ctx->J2, fd);

        fclose(fd);
    }
    gtk_widget_destroy (dialog);

    // et on met à jour tout ça sur l'IHM
    maj_etat(ctx);
}

void quitter_partie(GtkWidget *w, JE *ctx) {
    gtk_main_quit();
}

void action_pion(GtkWidget *widget, JE *ctx) {
    int i, j, x, y;

    // on cherche le bouton qui a déclenché ça
    if(widget == ctx->bt_cellules)
        y = -1;
    else if(widget == ctx->bt_sortie)
        y = 9;
    else    // evil loop
        for(j=0; j<9; j++)
            for(i=0; i<9; i++)
                if(widget == ctx->bts_cases[i][j]) {
                    x = i;
                    y = j;
                }

    // on fait l'action demandée en fonction de si on vient de choisir le pion ou sa destination
    if(jeu_etat(&ctx->jeu)&ETAT_ATTENTEBOUGER)
        jeu_bougerpion(&ctx->jeu, x, y);
    else
        jeu_selectpion(&ctx->jeu, x, y);

    // et on remet à jour l'IHM pour refléter les modifs
    maj_etat(ctx);
}

