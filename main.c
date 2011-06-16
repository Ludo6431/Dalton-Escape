#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JE/JE.h"

int main(int argc,char *argv[]) {
    GtkWidget *fenetre;
    JE ctx;
    bzero(&ctx, sizeof(ctx));

    // initialisation de gtk
    gtk_init(&argc, &argv);

    // on charge les styles
    gtk_rc_parse("style.rc");

    // création fenêtre du jeu
    fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fenetre), "Jeu d'évasion");
    g_signal_connect(G_OBJECT(fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    JE_ajout_nouveau(&ctx, GTK_WINDOW(fenetre));    // ajoute l'interface du jeu dans la fenêtre

    gtk_widget_show(fenetre);

    // let's go !
    gtk_main();

    return 0;
}

