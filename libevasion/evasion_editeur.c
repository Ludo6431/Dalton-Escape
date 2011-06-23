#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libevasion/evasion.h"

#include "libevasion/evasion_editeur.h"

static int canmove(EV *je, int sx, int sy, int dx, int dy) {
    assert(je);
    assert(sy!=9);  // on ne peut pas partir de la sortie

    if(sy == dy && (dy == -1 || sx == dx || dy == 9))    // on peut annuler notre déplacement en cliquant sur là où on était
        return 1;   // (rq. : si le y == -1 ou 9, le x peut valoir n'importe quoi)

    if(CASE_TYPE(ev_case_get(je, dx, dy)) != CASE_LIBRE)   // on ne peut pas aller là où il y a déjà qqu'un
        return 0;

    if(CASE_TYPE(ev_case_get(je, sx, sy)) == CASE_GARDIEN) // on déplace un gardien
        if(dy != sy)    // uniquement sur la même ligne
            return 0;

    return 1;
}

static void resetcliquer(EV *je) {
    int i, j;

    je->part[0] &= ~CASE_PEUTCLIQUER;
    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            je->tab[i][j] &= ~CASE_PEUTCLIQUER;
    je->part[1] &= ~CASE_PEUTCLIQUER;
}

void eved_debut_depl(EV *je, int sx, int sy) {    // choix de la source
    assert(je);
    assert(!(je->etat & ETAT_ATTENTEBOUGER));
    assert(ev_case_get(je, sx, sy) & CASE_PEUTCLIQUER);

    etat_t e = 0;

    switch(sy) {
    case -1:    // cellule
        switch(sx) {
        case 0:
            e = CASE2ETAT(CASE_J1);
            break;
        case 1:
            e = CASE2ETAT(CASE_J2);
            break;
        default:
            break;
        }
        break;
    default:
        e = CASE2ETAT(ev_case_get(je, sx, sy));
        break;
    }

    je->etat = ETAT_ATTENTEBOUGER | e;

    // on sauvegarde le point de départ
    je->sx = sx;
    je->sy = sy;
}

inline void eved_annuler_depl(EV *je) {
    ev_fin_depl(je, je->sx, je->sy);   // c'est comme si on bougeait là où on était
}

void eved_fin_depl(EV *je, int dx, int dy) {    // choix de la destination
    assert(je);
    assert(je->etat & ETAT_ATTENTEBOUGER);
    assert(ev_case_get(je, dx, dy) & CASE_PEUTCLIQUER);

    if(je->sy == dy && (dy == -1 || je->sx == dx || dy == 9)) {  // le joueur annule son déplacement => c'est encore à lui de jouer
        je->etat &= ~ETAT_ATTENTEBOUGER; // on n'est plus dans l'attente de sélection de la destination
        return; // on remet à jour les boutons qui sont cliquables sans changer de joueur
    }

    // on enlève le pion de là où il était
    ev_case_set(je, je->sx, je->sy, (ev_case_get(je, je->sx, je->sy)&~CASE_MASQTYPE) | CASE_LIBRE);

    if(EV_DANSCOUR(dx, dy))    // on met le pion là où il va
        je->tab[dx][dy] |= ETAT2CASE(je->etat);

    // on reset les flags (ETAT_ATTENTEBOUER, ...) stockés dans l'état
    je->etat &= ~ETAT_ATTENTEBOUGER;
}

void eved_maj_depl(EV *je) {
    int i, j;

    resetcliquer(je);   // on enlève les drapeaux

    if(je->etat&ETAT_ATTENTEBOUGER) {   // choix de la destination
        if(canmove(je, je->sx, je->sy, 0, -1))
            je->part[0] |= CASE_PEUTCLIQUER;

        for(j=0; j<9; j++)              // cour
            for(i=0; i<9; i++)
                if(canmove(je, je->sx, je->sy, i, j))
                    je->tab[i][j] |= CASE_PEUTCLIQUER;

        je->part[1] &= ~CASE_PEUTCLIQUER;   // inutile de pouvoir sortir
    }
    else {  // choix de la source
        je->part[0] |= CASE_PEUTCLIQUER;    // TODO : vérifier le nombre de pions restant pour le joueur qui va jouer (il faut stocker ça qque part)

        for(j=0; j<9; j++)
            for(i=0; i<9; i++)
                if(CASE_TYPE(je->tab[i][j]) != CASE_LIBRE)
                    je->tab[i][j] |= CASE_PEUTCLIQUER;

        je->part[1] &= ~CASE_PEUTCLIQUER;   // on ne peut pas récupérer les pions qui sont sortis
    }
}

