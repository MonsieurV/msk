/* NOYAUFILE.C */
/*--------------------------------------------------------------------------*
 *  gestion de la file d'attente des taches pretes et actives               *
 *  la file est rangee dans un tableau. ce fichier decrit toutes            *
 *  les primitives de base                                                  *
 *--------------------------------------------------------------------------*/
#include <stdint.h>
#include "serialio.h"
#include "noyau.h"

// Pour test.
#define DEBUG 1

/* variables communes a toutes les procedures *
 *--------------------------------------------*/

static uint16_t  _file[MAX_TACHES];   /* indice=numero de tache */
				   /* valeur=tache suivante  */
static uint16_t  _queue;              /* valeur de la derniere tache */
                        		   /* pointe la prochaine tache a activer */

/*     initialisation de la file      *
 *------------------------------------*
entre  : sans
sortie : sans
description : la queue est initialisee vide, queue prend la valeur de tache
	      impossible
*/

void	file_init( void )
{
    uint16_t i = 0;
	_queue = F_VIDE;
    for (i = 0; i < MAX_TACHES; i++) {
        _file[i] = F_VIDE;
    }
}

/*        ajouter une tache dans la pile      *
 *--------------------------------------------*
entree : n numero de la tache a entrer
sortie : sans
description : ajoute la tache n en fin de pile
*/

void	ajoute ( uint16_t n )
{
    uint16_t next_queue;
    // EXCEPTION : hors indice.
    if(n >= MAX_TACHES) {
        #if DEBUG
            return;
        #else
            _fatal_exception_("ajoute() : n >= MAX_TACHES");
        #endif
    }
    // CAS PARTICULIER : la pile est vide.
    if(_queue == F_VIDE) {
        _file[n] = n;
        _queue = n;
    }
    else {
        // CAS GENERAL : il y a déjà des tâches dans la pile.
        next_queue = _file[_queue];
        _file[_queue] = n;
        _file[n] = next_queue;
        _queue = n;
    }
}

/*           retire une tache de la file        *
 *----------------------------------------------*
entree : t numero de la tache a sortir
sortie : sans
description: sort la tache t de la file. L'ordre de la file n'est pas
	     modifie
*/

void	retire( uint16_t t )
{
    uint16_t i, previous_t;
    // EXCEPTION : hors indice.
    if(t >= MAX_TACHES) {
        #if DEBUG
            return;
        #else
            _fatal_exception_("retire() : t >= MAX_TACHES");
        #endif
    }
    // CAS GENERAL.
    // Recuperation tâche precedente
    for (i = 0; i < MAX_TACHES; i++) {
        if (_file[i] == t) {
            previous_t = i;
            break;
        }
    }
    _file[previous_t] = _file[t];
    _file[t] = F_VIDE;
    // CAS PARTICULIER : t == _queue et dernière tâche retirée.
    if(_queue == t) {
        if(previous_t == t)
            _queue = F_VIDE;
        _queue = previous_t;
    }
}


/*        recherche du suivant a executer       *
 *----------------------------------------------*
entree : sans
sortie : t numero de la tache a activer
description : la tache a activer est sortie de la file. queue pointe la
	      suivante
*/
uint16_t	suivant( void )
{
    uint16_t next = _queue;
    _queue = _file[_queue];
    return next;
}

/*     affichage du dernier element     *
 *--------------------------------------*
entree : sans
sortie : sans
description : affiche la valeur de queue
*/

void affic_queue( void )
{
    #if DEBUG
    printf("_queue : %hu \n\n", _queue);
    #endif
}

/*     affichage de la file     *
 *------------------------------*
entree : sans
sortie : sans
description : affiche les valeurs de la file
*/

void affic_file( void )
{
    #if DEBUG
    uint16_t i;
    printf("Indice : ");
    for (i = 0; i < MAX_TACHES; i++) {
        printf("%hu |", i);
    }
    printf("\n_file  : ");
    for (i = 0; i < MAX_TACHES; i++) {
        if(_file[i] == F_VIDE)
            printf("  |");
        else
            printf("%hu |", _file[i]);
    }
    printf("\n\n");
    #endif
}
