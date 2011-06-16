#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JE.h"

int main(int argc,char *argv[]) {
    GtkWidget *fenetre, *JE_wid;
    JE ctx;
    bzero(&ctx, sizeof(ctx));

    gtk_init(&argc,&argv);

    // création fenêtre du jeu
    fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fenetre), "Jeu d'évasion");
    g_signal_connect(G_OBJECT(fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    JE_wid = JE_nouveau(&ctx, GTK_WINDOW(fenetre));
    gtk_container_add(GTK_CONTAINER(fenetre), JE_wid);

    gtk_widget_show(fenetre);

    // let's go !
    gtk_main();

    return 0;
}

