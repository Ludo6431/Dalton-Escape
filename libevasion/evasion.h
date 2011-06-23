#ifndef _EVASION_H
#define _EVASION_H

#include <assert.h>
#include "commun/outils.h"

#define CASE_MASQTYPE (0x03)
#define CASE_TYPE(c) ((c)&CASE_MASQTYPE)
typedef enum {
    CASE_LIBRE          = 0,
    CASE_J1             = 1,
    CASE_J2             = 2,
    CASE_GARDIEN        = 3,
    CASE_GRISE          = BIT(2),
    CASE_PEUTCLIQUER    = BIT(3)
} case_t;

#define ETAT_MASQETAT (0x7)
#define ETAT_ETAT(e) ((e)&ETAT_MASQETAT)
typedef enum {
    ETAT_INIT           = 0,
    ETAT_J1             = 1,
    ETAT_J2             = 2,
    ETAT_GARDIEN        = 3,
    ETAT_J1WIN          = 4,
    ETAT_J2WIN          = 5,
    ETAT_ATTENTEBOUGER  = BIT(3),
    ETAT_ENREGCOUP      = BIT(4)
} etat_t;

#define CASE2ETAT(c) ((etat_t)CLAMP(CASE_J1, CASE_TYPE(c), CASE_GARDIEN))
#define ETAT2CASE(e) ((case_t)CLAMP(ETAT_J1, ETAT_ETAT(e), ETAT_GARDIEN))

typedef struct {
    int nb_pions[2];    // le nb de pions qui sont passés pour chaque joueur
    etat_t etat;        // état en cours
    int joueur_debut;   // joueur qui commence

    int sx, sy;         // position du pion sélectionné

    case_t tab[9][9];   // 81 cases du plateau
    case_t part[2];     // cases cellules et sortie
} EV;

void        ev_nouvellepartie   (EV *je);

void        ev_debut_depl       (EV *je, int x, int y);     // x appartient [0,8] ; y appartient [-1,8]
inline void ev_annuler_depl     (EV *je);
void        ev_fin_depl         (EV *je, int x, int y);     // x ppartient [0,8] ; y appartient [-1,9]
void        ev_maj_depl         (EV *je);

int         ev_sauvegarder      (EV *je, FILE *fd);
int         ev_charger          (EV *je, FILE *fd);

// helpers defines/functions
#define EV_DANSCELL(x, y) ((y)==-1)
#define EV_DANSCOUR(x, y) ((x)>=0 && (x)<9 && (y)>=0 && (y)<9)
#define EV_DANSSORT(x, y) ((y)==9)
#define EV_DANSPLAT(x, y) (EV_DANSCELL(x, y) || EV_DANSCOUR(x, y) || EV_DANSSORT(x, y))

static inline case_t ev_case_get(EV *je, int x, int y) {
    switch(y) {
    case -1:
        return je->part[0];
    case 9:
        return je->part[1];
    default:
//        assert(EV_DANSCOUR(x, y));
        return je->tab[x][y];
    }
}

static inline void ev_case_set(EV *je, int x, int y, case_t c) {
    switch(y) {
    case -1:
        je->part[0] = c;
        break;
    case 9:
        je->part[1] = c;
        break;
    default:
//        assert(EV_DANSCOUR(x, y));
        je->tab[x][y] = c;
        break;
    }
}

#endif

