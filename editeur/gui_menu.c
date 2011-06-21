#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commun/joueur.h"
#include "libevasion/evasion.h"
#include "editeur/gui_action.h"

#include "editeur/gui_menu.h"

// prototypes des callbacks
void    nouvelle_partie     (GtkWidget *w, JEU *ctx);
void    sauvegarder_partie  (GtkWidget *w, JEU *ctx);
void    charger_partie      (GtkWidget *w, JEU *ctx);
void    quitter_partie      (GtkWidget *w, JEU *ctx);

static GtkActionEntry menu_entries[] = {
    /* name, stock id, label, accelerator, tooltip, callback */
    {"GameMenuAction", NULL, "_Game"},
    {"NewAction", GTK_STOCK_NEW, "_New", "<control>N", "Start new game", G_CALLBACK(nouvelle_partie)},
    {"SaveAction", GTK_STOCK_SAVE_AS, "_Save", "<control>S", "Save game", G_CALLBACK(sauvegarder_partie)},
    {"LoadAction", GTK_STOCK_OPEN, "L_oad", "<control>O", "Load saved game", G_CALLBACK(charger_partie)},
    {"QuitAction", GTK_STOCK_QUIT, "_Quit", "<control>Q", "Quit game", G_CALLBACK(quitter_partie)},

    {"EditMenuAction", NULL, "E_dit"},
    {"UndoAction", GTK_STOCK_UNDO, "Undo", "<control>Z", "Undo last action", NULL},
    {"SettingsAction", GTK_STOCK_PREFERENCES, "Settings", NULL, "Save game", NULL},

    {"HelpMenuAction", NULL, "_Help"},
    {"ContentsAction", GTK_STOCK_DIALOG_QUESTION, "Contents", "F1", "Manual of the game", NULL},
    {"AboutAction", GTK_STOCK_ABOUT, "About", NULL, "About the game", NULL},
};

GtkWidget *gui_menu_new(GtkWindow *fenpar, void *user_ptr) {
    GError *error;
    GtkActionGroup *action_group;
    GtkUIManager *menu_manager;

    action_group = gtk_action_group_new("MenuActions");
    gtk_action_group_add_actions(action_group, menu_entries, G_N_ELEMENTS(menu_entries), user_ptr);
    menu_manager = gtk_ui_manager_new();
    gtk_ui_manager_insert_action_group(menu_manager, action_group, 0);
    error = NULL;
    gtk_ui_manager_add_ui_from_file(menu_manager, "jeu_menu.ui", &error);
    if(error) {
        g_message("building menus failed: %s", error->message);
        g_error_free(error);
    }
    gtk_window_add_accel_group(fenpar, gtk_ui_manager_get_accel_group(menu_manager));

    return gtk_ui_manager_get_widget(menu_manager, "/MainMenu");
}

// -- les callbacks :

void nouvelle_partie(GtkWidget *w, JEU *ctx) {
    joueur_init(&ctx->J1, "Nouveau joueur", "Joueur 1", GTK_WINDOW(ctx->gui.fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->gui.lbl_J1), ctx->J1.pseudo);
    joueur_init(&ctx->J2, "Nouveau joueur", "Joueur 2", GTK_WINDOW(ctx->gui.fenetre));
    gtk_label_set_label(GTK_LABEL(ctx->gui.lbl_J2), ctx->J2.pseudo);

    ev_nouvellepartie(&ctx->jeu);

    maj_etat(ctx);
}

#define EXT_FICHIER ".esc"

void sauvegarder_partie(GtkWidget *w, JEU *ctx) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Sauvegarder partie", GTK_WINDOW(ctx->gui.fenetre), GTK_FILE_CHOOSER_ACTION_SAVE,
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
        ev_sauvegarder(&ctx->jeu, fd);
        joueur_sauvegarder(&ctx->J1, fd);
        joueur_sauvegarder(&ctx->J2, fd);

        // TODO: append checksum ?

        fclose(fd);
    }
    gtk_widget_destroy (dialog);
}

void charger_partie(GtkWidget *w, JEU *ctx) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Charger partie", GTK_WINDOW(ctx->gui.fenetre), GTK_FILE_CHOOSER_ACTION_OPEN,
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
        ev_charger(&ctx->jeu, fd);
        joueur_charger(&ctx->J1, fd);
        joueur_charger(&ctx->J2, fd);

        fclose(fd);
    }
    gtk_widget_destroy (dialog);

    // et on met à jour tout ça sur l'IHM
    maj_etat(ctx);
}

void quitter_partie(GtkWidget *w, JEU *ctx) {
    gtk_main_quit();
}

