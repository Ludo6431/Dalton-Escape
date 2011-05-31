#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>

#include "joueur.h"

void player_init(JE_joueur *pl, char *msg, char *dpseudo, GtkWindow *top_win) {
    GtkWidget *dialog;
    GtkWidget *content_area;
        GtkWidget *table;
            GtkWidget *title_label;

            GtkWidget *txt_label;
            GtkWidget *text;

            GtkWidget *btc_label;
            GtkWidget *bt_color;

// build dialog
    dialog = gtk_dialog_new_with_buttons("Editer joueur", top_win,
         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_STOCK_OK,
         GTK_RESPONSE_NONE,
         NULL
    );

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        // table
        table = gtk_table_new(4, 2, FALSE /* homogeneous */);
        gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(table));

            // title
            title_label = gtk_label_new(msg);
            gtk_table_attach(GTK_TABLE(table), title_label, 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
            gtk_widget_show(title_label);

            // pseudo
                // text label
                txt_label = gtk_label_new("Pseudo :");
                gtk_table_attach(GTK_TABLE(table), txt_label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(txt_label);

                // text
                text = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(text), dpseudo);
                gtk_table_attach(GTK_TABLE(table), text, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(text);

            // color button
                // btc label
                btc_label = gtk_label_new("Color :");
                gtk_table_attach(GTK_TABLE(table), btc_label, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(btc_label);

                // color button
                bt_color = gtk_color_button_new();
                gtk_table_attach(GTK_TABLE(table), bt_color, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
                gtk_widget_show(bt_color);

        gtk_widget_show(table);

    gtk_dialog_run(GTK_DIALOG(dialog));

// get data
    strncpy(pl->pseudo, gtk_entry_get_text(GTK_ENTRY(text)), 19);
    pl->pseudo[19]=0;
    gtk_color_button_get_color(GTK_COLOR_BUTTON(bt_color), &pl->color);

    gtk_widget_destroy(dialog);
}

