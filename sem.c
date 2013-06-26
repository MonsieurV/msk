/* SEM.C */
/*--------------------------------------------------------------------------*
 *               Code C des primitives de sémaphore                         *
 *                                 SEM.C                                    *
 *--------------------------------------------------------------------------*/
 
#include "noyau.h""
#include "sem.h"

/*--------------------------------------------------------------------------*
 *            Variables internes du noyau                                   *
 *--------------------------------------------------------------------------*/
SEMAPHORE _sem[MAX_SEM];

/*--------------------------------------------------------------------------*
 *                  --- Initialise les sémaphores ---                       *
 * Entree : Neant                                                           *
 * Sortie : Neant                                                           *
 * Descrip: Initialise le tableau des sémaphores _sem :                     *
 *    - met le compteur d'éléments de chacune des sémaphore à -1            *
 *                                                                          *
 * Err. fatale: Neant                                                       *
 *                                                                          *
 *--------------------------------------------------------------------------*/
 void s_init ( void )
 {
    short i;
    /* initialisation de l'etat des sémaphores */
    for (i=0; i<MAX_SEM; i++)
        _sem[i].valeur = SEM_NCREE;
 }
 
/*--------------------------------------------------------------------------*
 *                  --- Créé une sémaphore ---                              *
 * Entree : Valeur initiale de la sémaphore                                 *
 * Sortie : Numéro de le sémaphore                                          *
 * Descrip: Initialise le tableau des sémaphores _sem :                     *
 *    - met le compteur d'éléments de chacune des sémaphore à -1            *
 *                                                                          *
 * Err. fatale:                                                             *
 *    - si v < 0                                                            *
 *    - s'il a déjà MAX_SEM sémaphores                                      *
 *                                                                          *
 *--------------------------------------------------------------------------*/
 ushort s_cree ( short v )
 {
    SEMAPHORE *p;
    static ushort sem = -1; /* contient numero dernier cree */
    
    /* numero de sémaphore suivante */
    sem++;
    /* sortie si depassement */
    if (sem >= MAX_SEM)
        _fatal_exception_("s_cree() : sem >= MAX_SEM");
    /* sortie si nombre de tâches */
    if (v >= MAX_TACHES)
        _fatal_exception_("s_cree() : v >= MAX_TACHES");
    // Initialiser la sémaphore.
    p = &_sem[sem];
    p->valeur = v;
    p->file.head = 0;
    p->file.queue = p->file.head;
    return sem;
 }
 
 /*--------------------------------------------------------------------------*
 *               --- Supprime une sémaphore ---                             *
 * Entree : Numéro de la sémaphore                                          *
 * Sortie : Néant                                                           *
 * Descrip: supprime le sémaphore n                                         *
 *                                                                          *
 * Err. fatale:                                                             *
 *    - si n >= MAX_SEM                                                     *
 *                                                                          *
 *--------------------------------------------------------------------------*/
void s_close ( ushort n )
{
    SEMAPHORE *p;
    /* sortie si depassement */
    if (n >= MAX_SEM)
    {
        _fatal_exception_("s_close() : n >= MAX_SEM");
    }
    // Réinitialiser la valeur de la sémaphore.
    p = &_sem[n];
    p->valeur = SEM_NCREE;
}

/*--------------------------------------------------------------------------*
 *                           --- P() ---                                    *
 * Entree : Numéro de la sémaphore                                          *
 * Sortie : Néant                                                           *
 * Descrip: Implémente P(s). Tente de prendre le sémaphore n                *
 *                                                                          *
 * Err. fatale:                                                             *
 *    - si n >= MAX_SEM                                                     *
 *    - si p->valeur == SEM_NCREE                                           *
 *    - si (tete+1)%MAX_TACHES == queue                                     *
 *                                                                          *
 *--------------------------------------------------------------------------*/
void s_wait ( ushort n )
{
    SEMAPHORE *p;
    
    /* Sortie si depassement */
    if (n >= MAX_SEM)
        _fatal_exception_("s_wait() : n >= MAX_SEM");
    p = &_sem[n];
    // Vérifier que le sémaphore est créé.
    if(p->valeur == SEM_NCREE)
        _fatal_exception_("s_wait() : p->valeur == SEM_NCREE");
    // Mettre à jour le sémaphore.
    /* Debut section critique */
    _lock_();
    p->valeur--;
    if(p->valeur < 0) {
        // Mettre en attente la tâche courante.
        p->file.tache[p->file.head] = _tache_c;
        p->file.head = (p->file.head+1) % MAX_TACHES;
    }
    /* Fin section critique */
    _unlock_();
    if(p->valeur < 0)
        dort();
}

/*--------------------------------------------------------------------------*
 *                           --- V() ---                                    *
 * Entree : Numéro de la sémaphore                                          *
 * Sortie : Néant                                                           *
 * Descrip: Implémente V(s). Libère le semaphore n                          *
 *                                                                          *
 * Err. fatale:                                                             *
 *    - si n >= MAX_SEM                                                     *
 *    - si p->valeur == SEM_NCREE                                           *
 *    - si (tete+1)%MAX_TACHES == queue                                     *
 *                                                                          *
 *--------------------------------------------------------------------------*/
void s_signal ( ushort n )
{
    SEMAPHORE *p;
    ushort tache;
    
    /* Sortie si depassement */
    if (n >= MAX_SEM)
        _fatal_exception_("s_wait() : n >= MAX_SEM");
    p = &_sem[n];
    // Vérifier que le sémaphore est créé.
    if(p->valeur == SEM_NCREE)
        _fatal_exception_("s_wait() : p->valeur == SEM_NCREE");
    // Mettre à jour le sémaphore.
     /* Debut section critique */
    _lock_();
    p->valeur++;
    if(p->valeur <= 0) {
        // Sortir une tâche de la file et la relancer.
        tache = p->file.tache[p->file.queue];
        p->file.queue = (p->file.queue+1) % MAX_TACHES;
    }
    /* Fin section critique */
    _unlock_();
    if(p->valeur <= 0)
        reveille(tache);
}
 