#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

#include "commun/io.h"

#define EXT_FICHIER ".esc"

void io_sauvegarder(GtkWindow *fenpar, char **old_filename, EV *ev, JOUEUR *J1, JOUEUR *J2) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Sauvegarder partie", GTK_WINDOW(fenpar), GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*"EXT_FICHIER);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    if(!*old_filename) {    // on n'a pas chargé de partie
//        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "~/");
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "partie"EXT_FICHIER);
    }
    else
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), *old_filename);

    if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if(*old_filename) free(*old_filename);
        *old_filename = filename;

        FILE *fd = fopen(filename, "wb+");
        if(!fd)
            printf("wtf!!\n");

        // TODO: add header ?

        // ordre important
        ev_sauvegarder(ev, fd);
        joueur_sauvegarder(J1, fd);
        joueur_sauvegarder(J2, fd);

        // TODO: append checksum ?

        fclose(fd);
    }
    gtk_widget_destroy (dialog);
}

void io_charger(GtkWindow *fenpar, char **old_filename, EV *ev, JOUEUR *J1, JOUEUR *J2) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Charger partie", GTK_WINDOW(fenpar), GTK_FILE_CHOOSER_ACTION_OPEN,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*"EXT_FICHIER);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if(*old_filename) free(*old_filename);
        *old_filename = filename;   // on garde une copie pour être au bon endroit au moment de sauvegarder

        FILE *fd = fopen(filename, "rb+");
        if(!fd)
            printf("wtf!?\n");

        // TODO: verify checksum (and header ?)

        // ordre important
        ev_charger(ev, fd);
        joueur_charger(J1, fd);
        joueur_charger(J2, fd);

        fclose(fd);
    }
    gtk_widget_destroy (dialog);
}

