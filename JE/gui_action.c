#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JE/joueur.h"
#include "JE/jeu.h"

#include "JE/gui_action.h"

void maj_appar_bouton(JE *ctx, GtkWidget *bt, case_t c) {
    GtkWidget *image = NULL;

    // changement de style
    if(c&CASE_GRISE)
        gtk_widget_set_name(bt, "case_grise");
    else
        gtk_widget_set_name(bt, "case");

    // changement d'état
    if(c&CASE_PEUTCLIQUER) {
        gtk_widget_set_sensitive(bt, TRUE);
    }
    else {
        gtk_widget_set_sensitive(bt, FALSE);
    }

    // changement d'image
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

    gtk_label_set_text(GTK_LABEL(ctx->gui.lbl_statut), buffer);

    // mise à jour des labels de scores
    sprintf(buffer, "%d", ctx->J1.score);
    gtk_entry_set_text(GTK_ENTRY(ctx->gui.score_J1), buffer);
    sprintf(buffer, "%d", ctx->J2.score);
    gtk_entry_set_text(GTK_ENTRY(ctx->gui.score_J2), buffer);

    // mise à jour du plateau
    maj_appar_bouton(ctx, ctx->gui.bt_cellules, ctx->jeu.part[0]);

    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            maj_appar_bouton(ctx, ctx->gui.bts_cases[i][j], ctx->jeu.tab[i][j]);

    maj_appar_bouton(ctx, ctx->gui.bt_sortie, ctx->jeu.part[1]);
}

void action_pion(GtkWidget *widget, JE *ctx) {
    int i, j, x, y;

    // on cherche le bouton qui a déclenché ça
    if(widget == ctx->gui.bt_cellules)
        y = -1;
    else if(widget == ctx->gui.bt_sortie)
        y = 9;
    else    // evil loop
        for(j=0; j<9; j++)
            for(i=0; i<9; i++)
                if(widget == ctx->gui.bts_cases[i][j]) {
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

