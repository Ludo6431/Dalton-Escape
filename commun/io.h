#ifndef _IO_H
#define _IO_H

#include "libevasion/evasion.h"
#include "commun/joueur.h"

void    io_sauvegarder  (GtkWindow *fenpar, char **old_filename, EV *ev, JOUEUR *J1, JOUEUR *J2);
void    io_charger      (GtkWindow *fenpar, char **old_filename, EV *ev, JOUEUR *J1, JOUEUR *J2);

#endif
