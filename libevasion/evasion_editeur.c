#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libevasion/evasion.h"

#include "libevasion/evasion_editeur.h"

static int canmove(EV *je, int sx, int sy, int dx, int dy) {
    assert(je);
    assert(sy!=9);  // on ne peut pas partir de la sortie

    if(sy == dy && dy == -1 && sx != dx)    // on ne peut pas bouger un pion des cellules d'un joueur à l'autre
        return 0;

    if(dy == -1 && ((CASE_TYPE(ev_case_get(je, sx, sy)) == CASE_J1 && dx == 1) || (CASE_TYPE(ev_case_get(je, sx, sy)) == CASE_J2 && dx == 0)))
        return 0;   // on ne peut pas bouger un pion vers les cellules d'un autre joueur

    if(sy == dy && (sx == dx || dy == 9))    // on peut annuler notre déplacement en cliquant sur là où on était
        return 1;   // (rq. : si le y == 9, le x peut valoir n'importe quoi)

    if(CASE_TYPE(ev_case_get(je, dx, dy)) != CASE_LIBRE)   // on ne peut pas aller là où il y a déjà qqu'un
        return 0;

    if(CASE_TYPE(ev_case_get(je, sx, sy)) == CASE_GARDIEN) // on déplace un gardien
        if(dy != sy)    // uniquement sur la même ligne
            return 0;

    return 1;
}

void eved_debut_depl(EV *je, int sx, int sy) {    // choix de la source
    assert(je);
    assert(!(je->etat & ETAT_ATTENTEBOUGER));
    assert(ev_case_get(je, sx, sy) & CASE_PEUTCLIQUER);

    // on stocke dans l'etat du jeu le type de case qu'on déplace
    etat_t e = 0;

    switch(sy) {
    case -1:    // cellules
        switch(sx) {
        case 0: // cellules J1
            e = CASE2ETAT(CASE_J1);
            break;
        case 1: // cellules J2
            e = CASE2ETAT(CASE_J2);
            break;
        default:
            break;
        }
        break;
    default:    // cour
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

    // le joueur annule son déplacement => c'est encore à lui de jouer
    if(je->sy == dy && (je->sx == dx || dy == 9)) {
        je->etat &= ~ETAT_ATTENTEBOUGER; // on n'est plus dans l'attente de sélection de la destination
        return;    // on ne change pas de joueur
    }

    // on enlève le pion de là où il était
    ev_case_set(je, je->sx, je->sy, (ev_case_get(je, je->sx, je->sy)&~CASE_MASQTYPE) | CASE_LIBRE);

    // on diminue le nombre de prisonniers en cellules quand on en sort un
    if(EV_DANSCELL(je->sx, je->sy)) {
        assert(je->nb_p_cell[je->sx]>0);
        je->nb_p_cell[je->sx]--;    // sx==0 si J1; sx==1 si J2
    }

    // on augmente le nombre de prisonniers en cellules quand on en remet un
    if(EV_DANSCELL(dx, dy)) {
        assert(je->nb_p_cell[dx]<4);
        je->nb_p_cell[dx]++;    // dx==0 si J1; dx==1 si J2
    }

    // on met le pion là où il va
    if(EV_DANSCOUR(dx, dy))
        je->tab[dx][dy] |= ETAT2CASE(je->etat);

    // on reset les flags (ETAT_ATTENTEBOUER, ...) stockés dans l'état
    je->etat &= ~ETAT_ATTENTEBOUGER;

    // on signale qu'un coup vient de se finir
    je->etat |= ETAT_ENREGCOUP;
}

void eved_maj_depl(EV *je) {
    int i, j;

    if(je->etat&ETAT_ATTENTEBOUGER) {   // choix de la destination
        if(canmove(je, je->sx, je->sy, 0, -1))  // cellules J1
            je->part[0] |= CASE_PEUTCLIQUER;
        else
            je->part[0] &= ~CASE_PEUTCLIQUER;

        if(canmove(je, je->sx, je->sy, 1, -1))  // cellules J2
            je->part[1] |= CASE_PEUTCLIQUER;
        else
            je->part[1] &= ~CASE_PEUTCLIQUER;

        for(j=0; j<9; j++)              // cour
            for(i=0; i<9; i++) {
                if(canmove(je, je->sx, je->sy, i, j))
                    je->tab[i][j] |= CASE_PEUTCLIQUER;
                else
                    je->tab[i][j] &= ~CASE_PEUTCLIQUER;
            }
    }
    else {  // choix de la source
        if(je->nb_p_cell[0]>0) // cellules J1
            je->part[0] |= CASE_PEUTCLIQUER;
        else
            je->part[0] &= ~CASE_PEUTCLIQUER;

        if(je->nb_p_cell[1]>0) // cellules J2
            je->part[1] |= CASE_PEUTCLIQUER;
        else
            je->part[1] &= ~CASE_PEUTCLIQUER;

        for(j=0; j<9; j++)
            for(i=0; i<9; i++) {
                if(CASE_TYPE(je->tab[i][j]) != CASE_LIBRE)
                    je->tab[i][j] |= CASE_PEUTCLIQUER;
                else
                    je->tab[i][j] &= ~CASE_PEUTCLIQUER;
            }
    }
}

