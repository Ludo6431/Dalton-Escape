#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libevasion/evasion.h"

void ev_nouvellepartie(EV *je) {
    assert(je);

    je->nb_p_cell[0] = je->nb_p_cell[1] = 4;
    je->nb_p_sort[0] = je->nb_p_sort[1] = 0;

    je->etat = (je->joueur_debut?ETAT_J2:ETAT_J1) | ETAT_ENREGCOUP;

    je->joueur_debut^=1;

    int i, j;
    je->part[0] = CASE_LIBRE;
    for(j=0; j<9; j++)
        for(i=0; i<9; i++)
            je->tab[i][j] = CASE_LIBRE;
    je->part[1] = CASE_LIBRE;

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
    for(i=0; i<sizeof(tab_gardiens)/sizeof(*tab_gardiens); i++)
        je->tab[tab_gardiens[i][0]][tab_gardiens[i][1]] = CASE_GARDIEN;
    for(i=0; i<sizeof(tab_grises)/sizeof(*tab_grises); i++)
        je->tab[tab_grises[i][0]][tab_grises[i][1]] |= CASE_GRISE;
}

static int freepath(EV *je, int sx, int sy, int dx, int dy) {
    assert(je);
    assert(EV_DANSCOUR(sx, sy) && (EV_DANSCOUR(dx, dy) || EV_DANSSORT(dx, dy)));

    if(EV_DANSSORT(dx, dy))   // be sure this is the good x coordinate
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

    if(dy==0 || EV_DANSSORT(dx, dy))
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

    if(CASE_TYPE(ev_case_get(je, dx, dy)) != CASE_LIBRE)   // on ne peut pas aller là où il y a déjà qqu'un
        return 0;

    if(EV_DANSSORT(dx, dy) && !(ev_case_get(je, sx, sy)&CASE_GRISE))  // il faut être sur une case grise pour sortir
        return 0;

    if(EV_DANSCELL(dx, dy))    // on peut revenir à tout moment aux cellules
        return 1;

    if(EV_DANSCELL(sx, sy)) {    // on part de la cellule
        if(dy==0)   // on peut aller à la colonne où on veut mais sur la première ligne
            return 1;
    }
    else {  // on part de la cour
        if(freepath(je, sx, sy, dx, dy))   // si la voie est libre et le sera, let's go
            return 1;
    }

    return 0;
}

void ev_debut_depl(EV *je, int sx, int sy) {    // choix de la source
    assert(je);
    assert(!(je->etat & ETAT_ATTENTEBOUGER));
    assert(ETAT_ETAT(je->etat)==ETAT_J1 || ETAT_ETAT(je->etat)==ETAT_J2);
    assert(ev_case_get(je, sx, sy) & CASE_PEUTCLIQUER);

    // on passe en état attente de sélection de destination
    je->etat |= ETAT_ATTENTEBOUGER;

    // si c'est pas fait, c'est trop tard maintenant !
    je->etat &= ~ETAT_ENREGCOUP;

    // on sauvegarde le point de départ
    je->sx = sx;
    je->sy = sy;
}

inline void ev_annuler_depl(EV *je) {
    ev_fin_depl(je, je->sx, je->sy);   // c'est comme si on bougeait là où on était
}

void ev_fin_depl(EV *je, int dx, int dy) {    // choix de la destination
    assert(je);
    assert(je->etat & ETAT_ATTENTEBOUGER);
    assert(ETAT_ETAT(je->etat)==ETAT_J1 || ETAT_ETAT(je->etat)==ETAT_J2);
    assert(ev_case_get(je, dx, dy) & CASE_PEUTCLIQUER);

    int joueur = ETAT_ETAT(je->etat)==ETAT_J1?0:1;  // joueur qui vient de bouger un pion

    // le joueur annule son déplacement => c'est encore à lui de jouer
    if(je->sy == dy && (dy == -1 || je->sx == dx || dy == 9)) {
        je->etat &= ~ETAT_ATTENTEBOUGER; // on n'est plus dans l'attente de sélection de la destination
        return;    // on ne change pas de joueur
    }

    // on enlève le pion de là où il était
    ev_case_set(je, je->sx, je->sy, (ev_case_get(je, je->sx, je->sy)&~CASE_MASQTYPE) | CASE_LIBRE);

    // on diminue le nombre de prisonniers en cellules quand on en sort un
    if(EV_DANSCELL(je->sx, je->sy)) {
        assert(je->nb_p_cell[joueur]>0);
        je->nb_p_cell[joueur]--;
    }

    // on augmente le nombre de prisonniers en cellules quand on en remet un
    if(EV_DANSCELL(dx, dy)) {
        assert(je->nb_p_cell[joueur]<4);
        je->nb_p_cell[joueur]++;
    }

    // le joueur a passé un pion dans la sortie
    if(EV_DANSSORT(dx, dy)) {
        je->nb_p_sort[joueur]++;

        int i, gx, ligne = (((unsigned)rand())%8)+1, dir = ((unsigned)rand())&1;

        // on cherche le gardien sur la ligne
        for(gx=0; gx<9; gx++)
            if(CASE_TYPE(je->tab[gx][ligne]) == CASE_GARDIEN)
                break;

        // on le fait aller à son nouvel emplacement en supprimant tous les pions sur son passage
        for(i=gx; (i<9 && dir) || (i>=0 && !dir); i+=(dir?1:-1)) {
            case_t c = CASE_TYPE(je->tab[i][ligne]);

            if(c==CASE_J1 || c==CASE_J2)
                je->nb_p_cell[CASE2JOUE(c)]++;  // si on écrase un joueur, on le renvoie dans la cellule

            je->tab[i][ligne] &= ~CASE_MASQTYPE;
        }

        je->tab[dir?8:0][ligne] |= CASE_GARDIEN;
    }

    if(je->nb_p_sort[joueur]>=3) {   // on vérifie si ce déplacement lui permet de gagner
        je->etat = (joueur?ETAT_J2WIN:ETAT_J1WIN) | ETAT_ENREGCOUP;
        return;
    }

    if(EV_DANSCOUR(dx, dy)) {  // on met le pion là où il va
        je->tab[dx][dy] |= joueur?CASE_J2:CASE_J1;
        if(dy>0) {  // on bouge le gardien qui va bien
            int gx, dist, i;

            // on cherche le gardien sur la ligne
            for(gx=0; gx<9; gx++)
                if(CASE_TYPE(je->tab[gx][dy]) == CASE_GARDIEN)
                    break;

            if(je->sx == dx)
                dist = (gx>je->sx?-1:1)*abs(dy-je->sy);
            else    // je->sy == dy
                dist = (gx>je->sx?-1:1)*abs(dx-je->sx);

            // on enlève le gardien de là où il était et les pions sur le passage du gardien
            for(i=0; (i<=dist && dist>0) || (i>=dist && dist<0); i+=(dist>0?1:-1)) {
                case_t c = CASE_TYPE(je->tab[CLAMP(0, gx+i, 8)][dy]);

                if(c==CASE_J1 || c==CASE_J2)
                    je->nb_p_cell[CASE2JOUE(c)]++;  // si on écrase un joueur, on le renvoie dans la cellule

                je->tab[CLAMP(0, gx+i, 8)][dy] &= ~CASE_MASQTYPE;
            }

            je->tab[CLAMP(0, gx+dist, 8)][dy] |= CASE_GARDIEN;  // on met le gardien à sa nouvelle place
        }
    }

    // au joueur suivant (et on reset aussi les flags (ETAT_ATTENTEBOUER, ...) stockés dans l'état)
    je->etat = (joueur?ETAT_J1:ETAT_J2) | ETAT_ENREGCOUP;
}

void ev_maj_depl(EV *je) {
    int i, j;

    if(je->etat&ETAT_ATTENTEBOUGER) {   // choix de la destination
        je->part[0] |= CASE_PEUTCLIQUER;    // on peut toujours aller en cellule

        for(j=0; j<9; j++)              // cour
            for(i=0; i<9; i++) {
                if(canmove(je, je->sx, je->sy, i, j))
                    je->tab[i][j] |= CASE_PEUTCLIQUER;
                else
                    je->tab[i][j] &= ~CASE_PEUTCLIQUER;
            }

        if(canmove(je, je->sx, je->sy, 0, 9))     // peut-on sortir ?
            je->part[1] |= CASE_PEUTCLIQUER;
        else
            je->part[1] &= ~CASE_PEUTCLIQUER;
    }
    else {  // choix de la source
        int joueur = ETAT_ETAT(je->etat)==ETAT_J1?0:1;  // joueur qui va jouer

        if(je->nb_p_cell[joueur]>0)
            je->part[0] |= CASE_PEUTCLIQUER;
        else
            je->part[0] &= ~CASE_PEUTCLIQUER;

        for(j=0; j<9; j++)
            for(i=0; i<9; i++) {
                if(CASE_TYPE(je->tab[i][j]) == (joueur?CASE_J2:CASE_J1))
                    je->tab[i][j] |= CASE_PEUTCLIQUER;
                else
                    je->tab[i][j] &= ~CASE_PEUTCLIQUER;
            }

        je->part[1] &= ~CASE_PEUTCLIQUER;   // on ne peut pas récupérer les pions qui sont sortis
    }
}

int ev_verifie(EV *je, int repare, int fichier) {
    int i, j;
    int cases[4] = {0}, nb;

    if(fichier && (ETAT_ETAT(je->etat)==ETAT_GARDIEN || ETAT_ETAT(je->etat)==ETAT_INIT))
        return 1;   // les états gardien et init sont utilisés en interne, ils ne doivent jamais se retrouver dans un fichier

    if(je->etat&ETAT_ATTENTEBOUGER) {
        int fail = 0;

        if(!EV_DANSPLAT(je->sx, je->sy) || EV_DANSSORT(je->sx, je->sy))
            fail = 2;

        if(EV_DANSCOUR(je->sx, je->sy) && (CASE_TYPE(ev_case_get(je, je->sx, je->sy))==CASE_LIBRE || (fichier && CASE_TYPE(ev_case_get(je, je->sx, je->sy))==CASE_GARDIEN)))
            fail = 3;

        if(fail) {
            if(repare)
                je->etat &= ~ETAT_ATTENTEBOUGER;
            else
                return fail;
        }
    }

    switch(ETAT_ETAT(je->etat)) {
    case ETAT_J1:
    case ETAT_J2:
    case ETAT_GARDIEN:
        for(j=0; j<9; j++) {    // on compte les éléments du tableau
            nb = cases[CASE_GARDIEN];

            for(i=0; i<9; i++)
                cases[CASE_TYPE(je->tab[i][j])]++;

            if(j>=1 && cases[CASE_GARDIEN]!=nb+1)
                return 4;   // irréparable
        }

        if(cases[CASE_J1]>4 || cases[CASE_J2]>4)    // il ne doit pas y avoir sur le plateau plus de 4 prisonniers de chaque joueur
            return 5;   // irréparable

        if(je->nb_p_cell[0]+cases[CASE_J1]+je->nb_p_sort[0]!=4) {   // il doit y avoir 4 joueurs exactement en tout pour chaque joueur
            if(repare)
                je->nb_p_cell[0] = 4-cases[CASE_J1]-je->nb_p_sort[0];
            else
                return 6;
        }

        if(je->nb_p_cell[1]+cases[CASE_J2]+je->nb_p_sort[1]!=4) {   // il doit y avoir 4 joueurs exactement en tout pour chaque joueur
            if(repare)
                je->nb_p_cell[1] = 4-cases[CASE_J2]-je->nb_p_sort[1];
            else
                return 7;
        }

        if(CASE_TYPE(je->part[0])!=CASE_LIBRE) {    // une case particuliaire ne contient pas de pion
            if(repare) {
                je->part[0] &= ~CASE_MASQTYPE;
                je->part[0] |= CASE_LIBRE;
            }
            else
                return 8;
        }

        if(CASE_TYPE(je->part[1])!=CASE_LIBRE) {    // une case particuliaire ne contient pas de pion
            if(repare) {
                je->part[1] &= ~CASE_MASQTYPE;
                je->part[1] |= CASE_LIBRE;
            }
            else
                return 9;
        }
        break;
    default:
        break;
    }

    return 0;
}

int ev_sauvegarder(EV *je, FILE *fd) {
    return fwrite(je, sizeof(*je), 1, fd)!=1;
}

int ev_charger(EV *je, FILE *fd) {
    return fread(je, sizeof(*je), 1, fd)!=1;
}

