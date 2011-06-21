#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libevasion/evasion.h"

void ev_nouvellepartie(EV *je) {
    assert(je);

    je->nb_pions[0] = je->nb_pions[1] = 0;

    je->etat = je->joueur_debut?ETAT_J2:ETAT_J1;

    je->joueur_debut^=1;

    int i, j;
    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            je->tab[i][j] = 0;
    je->part[0] = je->part[1] = 0;

    int tab_gardiens[8][2] = {  // position (x, y) initiale de chaque gardien
        {0, 1},
        {8, 2},
        {1, 3},
        {7, 4},
        {2, 5},
        {6, 6},
        {3, 7},
        {4, 8}
    };
    int tab_grises[9][2] = {    // position (x, y) de chaque case grise
        {4, 6},
        {3, 7},
        {4, 7},
        {5, 7},
        {2, 8},
        {3, 8},
        {4, 8},
        {5, 8},
        {6, 8}
    };
    for(i=0; i<8; i++)
        je->tab[tab_gardiens[i][0]][tab_gardiens[i][1]] = CASE_GARDIEN;
    for(i=0; i<9; i++)
        je->tab[tab_grises[i][0]][tab_grises[i][1]] |= CASE_GRISE;

    je->part[0] |= CASE_PEUTCLIQUER;
}

#define DANSCELL(x, y) ((y)==-1)
#define DANSCOUR(x, y) ((x)>=0 && (x)<9 && (y)>=0 && (y)<9)
#define DANSSORT(x, y) ((y)==9)
#define DANSPLAT(x, y) (DANSCELL(x, y) || DANSCOUR(x, y) || DANSSORT(x, y))

static case_t case_get(EV *je, int x, int y) {
    switch(y) {
    case -1:
        return je->part[0];
    case 9:
        return je->part[1];
    default:
        assert(DANSCOUR(x, y));
        return je->tab[x][y];
    }
}

static void case_set(EV *je, int x, int y, case_t c) {
    switch(y) {
    case -1:
        je->part[0] = c;
        break;
    case 9:
        je->part[1] = c;
        break;
    default:
        assert(DANSCOUR(x, y));
        je->tab[x][y] = c;
        break;
    }
}

static int freepath(EV *je, int sx, int sy, int dx, int dy) {
    assert(je);
    assert(DANSCOUR(sx, sy) && (DANSCOUR(dx, dy) || DANSSORT(dx, dy)));

    if(DANSSORT(dx, dy))   // be sure this is the good x coordinate
        dx = sx;

    int i, dist, xg = -1;

    if(sx == dx) {  // déplacement vertical
        dist = dy-sy;
        for(i=sy; (i<dy && dist>0) || (i>dy && dist < 0); i+=dist>0?1:-1)
            if(CASE_TYPE(je->tab[sx][i]) == CASE_GARDIEN)
                return 0;
    }
    else if(sy == dy) { // déplacement horizontal
        dist = dx-sx;
        for(i=sx; (i<dx && dist>0) || (i>dx && dist<0); i+=dist>0?1:-1)
            if(CASE_TYPE(je->tab[i][sy]) == CASE_GARDIEN)
                return 0;
    }
    else    // on ne peut pas combiner un déplacment vertical et horizontal
        return 0;

    if(dy==0 || DANSSORT(dx, dy))
        return 1;

    // il ne reste ici que les cas où on va vers une ligne où il y a un gardien (et on ne passe pas par dessus un gardien)
    for(i=0; i<9; i++)
        if(CASE_TYPE(je->tab[i][dy]) == CASE_GARDIEN)
            xg = i;
    assert(xg!=-1); // il y a forcément un gardien sur cette ligne

    if(abs(xg-dx) > abs(dist))   // si le gardien ne peut pas nous manger après être arrivé, ok
        return 1;

    return 0;
}

static int canmove(EV *je, int sx, int sy, int dx, int dy) {
    assert(je);
    assert(sy!=9);  // on ne peut pas partir de la sortie

    if(sy == dy && (dy == -1 || sx == dx || dy == 9))    // on peut annuler notre déplacement en cliquant sur là où on était
        return 1;   // (rq. : si le y == -1 ou 9, le x peut valoir n'importe quoi)

    if(CASE_TYPE(case_get(je, dx, dy)) != CASE_LIBRE)   // on ne peut pas aller là où il y a déjà qqu'un
        return 0;

    if(DANSSORT(dx, dy) && !(case_get(je, sx, sy)&CASE_GRISE))  // il faut être sur une case grise pour sortir
        return 0;

    if(DANSCELL(dx, dy))    // on peut revenir à tout moment aux cellules
        return 1;

    if(DANSCELL(sx, sy)) {    // on part de la cellule
        if(dy==0)   // on peut aller à la colonne où on veut mais sur la première ligne
            return 1;
    }
    else {  // on part de la cour
        if(freepath(je, sx, sy, dx, dy))   // si la voie est libre et le sera, let's go
            return 1;
    }

    return 0;
}

static void resetcliquer(EV *je) {
    int i, j;

    je->part[0] &= ~CASE_PEUTCLIQUER;
    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            je->tab[i][j] &= ~CASE_PEUTCLIQUER;
    je->part[1] &= ~CASE_PEUTCLIQUER;
}

void ev_debut_depl(EV *je, int sx, int sy) {    // choix de la source
    assert(je);
    assert(!(je->etat & ETAT_ATTENTEBOUGER));
    assert(ETAT_ETAT(je->etat)==ETAT_J1 || ETAT_ETAT(je->etat)==ETAT_J2);
    assert(case_get(je, sx, sy) & CASE_PEUTCLIQUER);

    je->etat |= ETAT_ATTENTEBOUGER;

    // on sauvegarde le point de départ
    je->sx = sx;
    je->sy = sy;

    // ----
    // on met à jour les cases cliquables en fonction de la possibilité du coup

    resetcliquer(je);   // on enlève les drapeaux

    int i, j;

    je->part[0] |= CASE_PEUTCLIQUER;    // on peut toujours aller aux cellules

    for(j=0; j<9; j++)              // cour
        for(i=0; i<9; i++)
            if(canmove(je, sx, sy, i, j))
                je->tab[i][j] |= CASE_PEUTCLIQUER;

    if(canmove(je, sx, sy, 0, 9))     // sortie
        je->part[1] |= CASE_PEUTCLIQUER;
}

inline void ev_annuler_depl(EV *je) {
    ev_fin_depl(je, je->sx, je->sy);   // c'est comme si on bougeait là où on était
}

void ev_fin_depl(EV *je, int dx, int dy) {    // choix de la destination
    assert(je);
    assert(je->etat & ETAT_ATTENTEBOUGER);
    assert(ETAT_ETAT(je->etat)==ETAT_J1 || ETAT_ETAT(je->etat)==ETAT_J2);
    assert(case_get(je, dx, dy) & CASE_PEUTCLIQUER);

    int joueur = ETAT_ETAT(je->etat)==ETAT_J1?0:1;  // joueur qui vient de bouger un pion

    if(je->sy == dy && (dy == -1 || je->sx == dx || dy == 9)) {  // le joueur annule son déplacement => c'est encore à lui de jouer
        je->etat &= ~ETAT_ATTENTEBOUGER; // on n'est plus dans l'attente de sélection de la destination
        goto update;    // on remet à jour les boutons qui sont cliquables sans changer de joueur
    }

    // on enlève le pion de là où il était
    case_set(je, je->sx, je->sy, (case_get(je, je->sx, je->sy)&~CASE_MASQTYPE) | CASE_LIBRE);

    if(DANSSORT(dx, dy))    // le joueur a passé un pion dans la sortie
        je->nb_pions[joueur]++;

    if(je->nb_pions[joueur]==3) {   // on vérifie si ce déplacement lui permet de gagner
        je->etat = joueur?ETAT_J2WIN:ETAT_J1WIN;
        goto update;
    }

    if(DANSCOUR(dx, dy)) {  // on met le pion là où il va
        je->tab[dx][dy] |= joueur?CASE_J2:CASE_J1;
        if(dy>0) {  // on bouge le gardien qui va bien
            int gx, dist, i;
            for(gx=0; gx<9; gx++)
                if(CASE_TYPE(je->tab[gx][dy]) == CASE_GARDIEN)
                    break;

            if(je->sx == dx)
                dist = (gx>je->sx?-1:1)*abs(dy-je->sy);
            else    // je->sy == dy
                dist = (gx>je->sx?-1:1)*abs(dx-je->sx);

            // on enlève le gardien de là où il était et les pions sur le passage du gardien
            for(i=0; (i<=dist && dist>0) || (i>=dist && dist<0); i+=(dist>0?1:-1))
                je->tab[CLAMP(0, gx+i, 8)][dy] = je->tab[CLAMP(0, gx+i, 8)][dy]&~CASE_MASQTYPE;
            je->tab[CLAMP(0, gx+dist, 8)][dy] |= CASE_GARDIEN;  // on met le gardien à sa nouvelle place
        }
    }

    // au joueur suivant (et on reset aussi les flags (ETAT_ATTENTEBOUER, ...) stockés dans l'état)
    je->etat = joueur?ETAT_J1:ETAT_J2;
    joueur ^= 1;

update:
    // ----
    // on met à jour les cases cliquables en fonction de l'appartenance des pions

    resetcliquer(je);   // on enlève les drapeaux

    je->part[0] |= CASE_PEUTCLIQUER;    // TODO : vérifier le nombre de pions restant pour le joueur qui va jouer (il faut stocker ça qque part)

    int i, j;

    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            if(CASE_TYPE(je->tab[i][j]) == (joueur?CASE_J2:CASE_J1))
                je->tab[i][j] |= CASE_PEUTCLIQUER;

    je->part[1] &= ~CASE_PEUTCLIQUER;
}

int ev_sauvegarder(EV *je, FILE *fd) {
    return fwrite(je, sizeof(*je), 1, fd)!=1;
}

int ev_charger(EV *je, FILE *fd) {
    return fread(je, sizeof(*je), 1, fd)!=1;
}

