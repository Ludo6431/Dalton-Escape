#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commun/joueur.h"
#include "libevasion/evasion.h"

#include "jeu/gui_action.h"

enum {
    TARGET_PAWN
};

static GtkTargetEntry target_list[] = {
    { "PAWN",    0, TARGET_PAWN }
};

static void maj_appar_bouton(JEU *ctx, GtkWidget *bt, case_t c) {
    GtkWidget *image = NULL;

    // changement de style
    if(c&CASE_GRISE)
        gtk_widget_set_name(bt, "case_grise");
    else
        gtk_widget_set_name(bt, "case");

    // changement d'état
    if(c&CASE_PEUTCLIQUER) {
        if(ctx->jeu.etat&ETAT_ATTENTEBOUGER) {
            gtk_drag_dest_set(
                bt,                         /* widget that will accept a drop */
                GTK_DEST_DEFAULT_MOTION     /* default actions for dest on DnD */
                | GTK_DEST_DEFAULT_HIGHLIGHT,
                target_list,                /* lists of target to support */
                G_N_ELEMENTS(target_list),  /* size of list */
                GDK_ACTION_COPY             /* what to do with data after dropped */
            );
        }
        else {
            gtk_drag_source_set(
                bt,                         /* widget will be drag-able */
                GDK_BUTTON1_MASK,           /* modifier that will start a drag */
                target_list,                /* lists of target to support */
                G_N_ELEMENTS(target_list),  /* size of list */
                GDK_ACTION_COPY             /* what to do with data after dropped */
            );
        }

        gtk_widget_set_sensitive(bt, TRUE);
    }
    else {
        gtk_drag_source_unset(bt);
        gtk_drag_dest_unset(bt);

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

void maj_etat(JEU *ctx) {
    char buffer[64];
    int i, j;

    // on demande la mise à jour des déplacements possibles
    ev_maj_depl(&ctx->jeu);

    // on empile le contexte actuel si un coup vient de se finir
    if(ctx->jeu.etat&ETAT_ENREGCOUP) {
        ctx->jeu.etat &= ~ETAT_ENREGCOUP;   // acknowledge

        jeu_empile_coup(ctx);
    }

    // mise à jour du label d'état
    switch(ETAT_ETAT(ctx->jeu.etat)) {
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
        sprintf(buffer, "Autre état : %d", ctx->jeu.etat);
        break;
    }

    if(ctx->jeu.etat&ETAT_ATTENTEBOUGER)
        strcat(buffer, " (choisir la destination)");

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

static int _w2xy(GtkWidget *w, JEU *ctx, int *x, int *y) {
    int i, j;

    if(w == ctx->gui.bt_cellules) {
        *x = 0;
        *y = -1;
        return 0;
    }
    else if(w == ctx->gui.bt_sortie) {
        *y = 9;
        return 0;
    }
    else    // evil loop
        for(j=0; j<9; j++)
            for(i=0; i<9; i++)
                if(w == ctx->gui.bts_cases[i][j]) {
                    *x = i;
                    *y = j;
                    return 0;
                }

    return 1;   // le bouton n'est pas ici
}

void debut_drag(GtkWidget *widget, GdkDragContext *context, JEU *ctx) {
    char *stock = NULL;
    gint w, h;
    int x, y;

    // debug
/*    const gchar *name = gtk_widget_get_name(widget);*/
/*    g_print("%s: %s\n", name, __func__);*/

    // on récupère les coordonnées du bouton appuyé
    _w2xy(widget, ctx, &x, &y);

    // on renseigne le jeu du déplacement
    ev_debut_depl(&ctx->jeu, x, y);

    // on met à jour l'IHM
    maj_etat(ctx);


    // on configure le drag&drop
    gtk_icon_size_lookup(GTK_ICON_SIZE_DND, &w, &h);

    switch(ETAT_ETAT(ctx->jeu.etat)) {
    case ETAT_J1:
        stock = "case-J1";
        break;
    case ETAT_J2:
        stock = "case-J2";
        break;
    default:
        break;
    }

    gtk_drag_set_icon_stock(context, stock, w/2, h/2);

    gtk_button_set_image(GTK_BUTTON(widget), NULL);
}

gboolean drop_drag(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, JEU *ctx) {
    GdkAtom target_type;

    // debug
/*    const gchar *name = gtk_widget_get_name (widget);*/
/*    g_print("%s: %s\n", name, __func__);*/

    /* If the source offers a target */
    if(context->targets) {
        /* Choose the best target type */
        target_type = GDK_POINTER_TO_ATOM(g_list_nth_data(context->targets, TARGET_PAWN));

        /* Request the data from the source. */
        gtk_drag_get_data(
            widget,         /* will receive 'drag-data-received' signal */
            context,        /* represents the current state of the DnD */
            target_type,    /* the target type we want */
            time            /* time stamp */
        );
    }
    /* No target offered by source => error */
    else
        return FALSE;

    return TRUE;
}

#define DONNEE_DRAG "deplacement"

void demande_donnee_drag(GtkWidget *widget, GdkDragContext *context, GtkSelectionData *selection_data, guint target_type, guint time, JEU *ctx) {
    const gchar *pawn_data = DONNEE_DRAG;

    // debug
/*    const gchar *name = gtk_widget_get_name(widget);*/
/*    g_print("%s: %s\n", name, __func__);*/

    g_assert(selection_data!=NULL);
    g_assert(target_type==TARGET_PAWN);

    switch(target_type) {
    /* case TARGET_SOME_OBJECT:
     * Serialize the object and send as a string of bytes.
     * Pixbufs, (UTF-8) text, and URIs have their own convenience
     * setter functions */

    case TARGET_PAWN:
        gtk_selection_data_set(
            selection_data,
            selection_data->target,
            8,
            (guchar*)pawn_data,
            strlen(pawn_data)
        );
        break;
    default:
        /* Default to some a safe target instead of fail. */
        g_assert_not_reached ();
    }
}

void recoit_donnee_drag(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *selection_data, guint target_type, guint time, JEU *ctx) {
    gboolean success = TRUE;

    // debug
/*    const gchar *name = gtk_widget_get_name(widget);*/
/*    g_print("%s: %s\n", name, __func__);*/

    // on récupère les coordonnées de la destination
    if(_w2xy(widget, ctx, &ctx->dx, &ctx->dy))
        success = FALSE;

    /* Deal with what we are given from source */
    if(selection_data && selection_data->length>=0) {
        /* Check that we got the format we can use */
        switch(target_type) {
            case TARGET_PAWN:
//                g_print("received:%s\n", (gchar*)selection_data->data);
                if(strcmp((char *)selection_data->data, DONNEE_DRAG))
                    success = FALSE;
                break;
            default:
                break;
        }
    }

    gtk_drag_finish(context, success, FALSE, time);
}

void fin_drag(GtkWidget *widget, GdkDragContext *context, JEU *ctx) {
    // debug
/*    const gchar *name = gtk_widget_get_name(widget);*/
/*    g_print("%s: %s\n", name, __func__);*/

    if(context->action) { // le glisser-déposer a bien fonctionné
        // on effectue l'action
        ev_fin_depl(&ctx->jeu, ctx->dx, ctx->dy);
    }
    else {
        // on en renseigne le jeu
        ev_annuler_depl(&ctx->jeu);
    }

    // on màj l'IHM
    maj_etat(ctx);
}

void action_pion(GtkWidget *widget, JEU *ctx) {
    int x, y;

    // on cherche le bouton qui a déclenché ça
    _w2xy(widget, ctx, &x, &y);

    // on fait l'action demandée en fonction de si on vient de choisir le pion ou sa destination
    if(ctx->jeu.etat&ETAT_ATTENTEBOUGER)
        ev_fin_depl(&ctx->jeu, x, y);
    else
        ev_debut_depl(&ctx->jeu, x, y);

    // et on remet à jour l'IHM pour refléter les modifs
    maj_etat(ctx);
}

