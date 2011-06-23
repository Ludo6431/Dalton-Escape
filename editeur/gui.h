#ifndef _GUI_H
#define _GUI_H

// éléments de l'interface graphique
typedef struct {
    GtkWidget *fenetre;
        GtkWidget *vbox;
            GtkWidget *menu;
            GtkWidget *aframe;
                GtkWidget *table;
                    GtkWidget *bt_cellules[2];
                    GtkWidget *bts_cases[9][9];
                    GtkWidget *lbl_J1, *score_J1;
                    GtkWidget *bt_sortie;
                    GtkWidget *lbl_J2, *score_J2;
            GtkWidget *lbl_statut;
} GUI;

GtkWidget *gui_init(GUI *gui, GtkWindow *fenpar, void *user_ptr);

#endif

