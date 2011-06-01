#ifndef _EVASION_H
#define _EVASION_H

#include "outils.h"

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
    ETAT_INIT             = 0,
    ETAT_J1               = 1,
    ETAT_J2               = 2,
    ETAT_J1WIN            = 3,
    ETAT_J2WIN            = 4,
    ETAT_ATTENTEBOUGER    = BIT(3)
} etat_t;

typedef struct {
    int nb_pions[2];    // le nb de pions qui sont passés pour chaque joueur
    etat_t etat;        // état en cours
    int joueur_debut;   // joueur qui commence

    int sx, sy;         // position du pion sélectionné

    case_t tab[9][9];   // 81 cases du plateau
    case_t part[2];     // cases cellules et sortie
} JE_jeu;

etat_t  JE_etat             (JE_jeu *je);
void    JE_nouvellepartie   (JE_jeu *je);
void    JE_selectpion       (JE_jeu *je, int x, int y);     // x appartient [0,8] ; y appartient [-1,8]
void    JE_bougerpion       (JE_jeu *je, int x, int y);     // x ppartient [0,8] ; y appartient [-1,9]

void    JE_sauverpartie     (JE_jeu *je, FILE *fd);
void    JE_chargerpartie    (JE_jeu *je, FILE *fd);

#endif

