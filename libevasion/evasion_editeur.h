#ifndef _EVASION_EDITEUR_H
#define _EVASION_EDITEUR_H

#include "libevasion/evasion.h"

void        eved_debut_depl     (EV *je, int x, int y);     // x appartient [0,8] ; y appartient [-1,8]
inline void eved_annuler_depl   (EV *je);
void        eved_fin_depl       (EV *je, int x, int y);     // x ppartient [0,8] ; y appartient [-1,9]
void        eved_maj_depl       (EV *je);

#endif

