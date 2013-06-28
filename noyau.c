/* NOYAU.C */
/*--------------------------------------------------------------------------*
 *               Code C du noyau preemptif qui tourne sur ARM                *
 *                                 NOYAU.C                                  *
 *--------------------------------------------------------------------------*/

#include <stdint.h>

#include "serialio.h"
#include "imx_timers.h"
#include "imx_aitc.h"
#include "noyau.h"

/*--------------------------------------------------------------------------*
 *            Variables internes du noyau                                   *
 *--------------------------------------------------------------------------*/
static int compteurs[MAX_TACHES]; /* Compteurs d'activations */
CONTEXTE _contexte[MAX_TACHES];   /* tableau des contextes */
volatile uint16_t _tache_c;       /* numéro de tache courante */
uint32_t  _tos;                   /* adresse du sommet de pile */
int  _ack_timer = 1;              /* = 1 si il faut acquitter le timer */

/*--------------------------------------------------------------------------*
 *                        Fin de l'execution                                *
 *----------------------------------------------------------------------- --*/
void	noyau_exit(void)
{
    ushort i;
    /* Désactiver les interruptions */
    _irq_disable_();
    printf("Sortie du noyau MSK\n");
    /* Afficher par exemple le nombre d'activation de chaque tache */
    for(i=0; i<MAX_TACHES; i++)
        printf("%d ------> %d\n", i, compteurs[i]);
    printf("\n");
    /* Terminer l'exécution */
    _exit_();
}

/*--------------------------------------------------------------------------*
 *                        --- Fin d'une tache ---                           *
 * Entree : Neant                                                           *
 * Sortie : Neant                                                           *
 * Descrip: Cette proc. doit etre appelee a la fin des taches               *
 *                                                                          *
 *----------------------------------------------------------------------- --*/
void  fin_tache(void)
{
    /* on interdit les interruptions */
    _irq_disable_();
    /* la tache est enlevee de la file des taches */
    _contexte[_tache_c].status = CREE;
    retire(_tache_c);
    schedule();
}

/*--------------------------------------------------------------------------*
 *                        --- Creer une tache ---                           *
 * Entree : Adresse de la tache                                             *
 * Sortie : Numero de la tache creee                                        *
 * Descrip: Cette procedure cree une tache en lui allouant                  *
 *    - une pile (deux piles en fait une pour chaque mode du processeur)    *
 *    - un numero                                                           *
 * Err. fatale: priorite erronnee, depassement du nb. maximal de taches     *
 *	                                                                        *
 *--------------------------------------------------------------------------*/
uint16_t cree( TACHE_ADR adr_tache )
{
    CONTEXTE *p;                    /* pointeur d'une case de _contexte */
    static   uint16_t tache = -1;   /* contient numero dernier cree */
    /* debut section critique */
    _lock_();
    /* numero de tache suivant */
    tache++;
    /* Sortie car depassement  */
    if (tache >= MAX_TACHES)
        _fatal_exception_("cree() : tache >= MAX_TACHES");        
    /* contexte de la nouvelle tache */
    p = &_contexte[tache];
    /* allocation d'une pile a la tache */
    p->sp_ini = _tos;
    p->sp_irq = _tos - PILE_TACHE;
    /* decrementation du pointeur de pile pour la prochaine tache. */
    _tos = _tos - PILE_IRQ - PILE_TACHE;
    /* fin section critique */
    _unlock_();
    /* memorisation adresse debut de tache */
    p->tache_adr = adr_tache;
    /* mise a l'etat CREE */
    p->status= CREE;
    return (tache);                  /* tache est un uint16_t */
}

/*--------------------------------------------------------------------------*
 *                          --- Elire une tache ---                         *
 * Entree : Numero de la tache                                              *
 * Sortie : Neant                                                           *
 * Descrip: Cette procedure place une tache dans la file d'attente des      *
 *	    taches eligibles.                                                   *
 *	    Les activations ne sont pas memorisee                               *
 * Err. fatale: Tache inconnue	                                            *
 *                                                                          *
 *--------------------------------------------------------------------------*/
void  active( uint16_t tache )
{
    CONTEXTE *p = &_contexte[tache]; /* acces au contexte tache */
    if (p->status == NCREE)
        _fatal_exception_("active() : p->status == NCREE");
    /* debut section critique */
    _lock_();
    if (p->status == CREE)          	/* n'active que si receptif */
    {
        /* changement d'etat, mise a l'etat PRET */
        p->status = PRET;
        /* ajouter la tache dans la liste */
        ajoute(tache);
        /* activation d'une tache prete */
        schedule();    
    }
    /* fin section critique */
    _unlock_();
}


/*--------------------------------------------------------------------------*
 *                  ORDONNANCEUR preemptif optimise                         *
 *                                                                          *
 *             !! Cette fonction doit s'exécuter en mode IRQ !!             *
 *  !! Pas d'appel direct ! Utiliser schedule pour provoquer une            *
 *  commutation !!                                                          *
 *--------------------------------------------------------------------------*/
void __attribute__((naked)) scheduler( void )
{
    register CONTEXTE *p;
    register unsigned int sp asm("sp");  /* Pointeur de pile */
    /* Sauvegarder le contexte complet sur la pile IRQ */
    __asm__ __volatile__(
        /* Sauvegarde registres mode system */
        "stmfd sp, {r0-r14}^\t\n"
    	/* Attendre un cycle */
        "nop\t\n"
        /* Ajustement pointeur de pile */
        "sub sp, sp, #60\t\n"
    	/* Sauvegarde de spsr_irq */
        "mrs  r0, spsr\t\n"
        "stmfd sp!, {r0}\t\n"
        /* et de lr_irq */
        "stmfd sp!, {lr}\t\n"
    );			
    if (_ack_timer)                 /* Réinitialiser le timer si nécessaire */
    {
    	/* Acquiter l'événement de comparaison du Timer pour pouvoir */
    	/* obtenir le déclencement d'une prochaine interruption */
        struct imx_timer* tim1 = (struct imx_timer *) TIMER1_BASE;
        tim1->tstat &= ~TSTAT_COMP;
    }
    else
    {
        _ack_timer = 1;
    }
    /* memoriser le pointeur de pile */
    _contexte[_tache_c].sp_irq = sp;
    /* recherche du suivant */
    _tache_c = suivant();
    /* Incrémenter le compteur d'activations  */
    compteurs[_tache_c]++;
    /* p pointe sur la nouvelle tache courante*/
    p = &_contexte[_tache_c];
    if (p->status == PRET)          /* tache prete ? */
    {
    	/* Charger sp_irq initial */
        sp = p->sp_irq;
    	/* Passer en mode système */
        _set_arm_mode_(ARMMODE_SYS);
    	/* Charger sp_sys initial */
        sp = p->sp_ini;
    	/* status tache -> execution */
        p->status = EXEC;
    	/* autoriser les interuptions   */
        _irq_enable_();
    	/* lancement de la tâche */
        *(p->tache_adr)(); 
    }
    else
    {
    	/* tache deja en execution, restaurer sp_irq */
        sp = p->sp_irq;
    }
    /* Restaurer le contexte complet depuis la pile IRQ */
     __asm__ __volatile__(
    	/* Restaurer lr_irq */
        "ldmfd sp!, {lr}\t\n"
    	/* et spsr_irq */
        "ldmfd sp!, {r0}\t\n"
        "msr  spsr, r0\t\n"
    	/* Restaurer registres mode system */
        /* On met depile les elements contenu dans la pile pour les affecter aux registre r1 à r12*/
        "ldmfd sp, {r0-r14}^\t\n"
    	/* Attendre un cycle */
        "nop\t\n"
    	/* Ajuster pointeur de pile irq */
        /* Ajout de 60 pour ajuster la pile irq, ldmfd a modifie le pointeur de pile usr/sys. */
        "add sp, sp, #60\t\n"
        /* Retour d'exception */
        "subs pc, lr, #4\t\n"
    	);
}


/*--------------------------------------------------------------------------*
 *                  --- Provoquer une commutation ---                       *
 *                                                                          *
 *             !! Cette fonction doit s'exécuter en mode IRQ !!             *
 *  !! Pas d'appel direct ! Utiliser schedule pour provoquer une            *
 *  commutation !!                                                          *
 *--------------------------------------------------------------------------*/
void  schedule( void )
{
    /* Debut section critique */
    _lock_();
    _ack_timer = 0;
    /* On simule une exception irq pour forcer un appel correct à scheduler().*/
    /* Passer en mode IRQ */
    _set_arm_mode_(ARMMODE_IRQ);
    __asm__ __volatile__(
    	/* Sauvegarder cpsr dans spsr */
        "mrs  r0, cpsr\t\n"
    	"msr  spsr, r0\t\n"
    	/* Sauvegarder pc dans lr et l'ajuster */
        "mov  lr, pc\t\n"
        "add  lr, #12\t\n"
        /* Saut à scheduler */
        "b scheduler\t\n"
    );
    /* Repasser en mode system */
    _set_arm_mode_(ARMMODE_SYS);
    /* Fin section critique */
    _unlock_();
}

/*--------------------------------------------------------------------------*
 *                        --- Lancer le systeme ---                         *
 * Entree : Adresse de la premiere tache a lancer                           *
 * Sortie : Neant                                                           *
 * Descrip: Active la tache et lance le systeme                             *
 *                                                                          *
 *                                                                          *
 * Err. fatale: Neant                                                       *
 *                                                                          *
 *--------------------------------------------------------------------------*/
void	start( TACHE_ADR adr_tache )
{
    short j;
    register unsigned int sp asm("sp");
    struct imx_timer* tim1 = (struct imx_timer *) TIMER1_BASE;
    struct imx_aitc* aitc = (struct imx_aitc *) AITC_BASE;
    for (j=0; j<MAX_TACHES; j++)
    {
        /* initialisation de l'etat des taches */
        _contexte[j].status = NCREE;
    }
    /* initialisation de la tache courante */
    _tache_c = 0;
    /* initialisation de la file           */
    file_init();
    /* Initialisation de la variable Haut de la pile des tâches */
    _tos = sp;
    /* Passer en mode IRQ */
    _set_arm_mode_(ARMMODE_IRQ);
    /* sp_irq initial */
    sp = _tos;
    /* Repasser en mode SYS */
    _set_arm_mode_(ARMMODE_SYS);
    /* on interdit les interruptions */
    _irq_disable_();
    /* Initialisation du timer à 100 Hz */
    // YTO/TBE : TCTCL1 : SWR = 0 - FRR = 0 - CAP = 00 - OM = 0 - IRQEN = 1 - CLKSOURCE = 010 - TEN = 1
    // TODO Faire avec des maks.
    // tim1->tctl |= TCTL_SWR | TCTL_FRR | TCTL_CAP_ANY | TCTL_OM;
    // tim1->tctl = tim1->tctl & ~TCTL_IRQEN;
    // tim1->tctl = tim1->tctl | TCTL_CLKSOURCE_PERCLK;
    tim1->tctl = 0x15;
    // YTO/TBE : TPRER : 0X64 = 100 decimal
    tim1->tprer = 0x64;
    // YTO/TBE : TCMP1 : 0X64 = 100 decimal
    tim1->tcmp = 0x64;
    /* Initialisation de l'AITC */
    aitc->nimask = 0x1F;
    aitc->inttypeh = 0x08000000;
    aitc->inttypel = 0;
    aitc->nipriority[7] = 0xF000;
    // Activer la gestion des exceptions.
    aitc->intenableh = 0x08000000;
    aitc->intenablel = 0;
    /* creation et activation premiere tache */
    active(cree(adr_tache));
}


/*-------------------------------------------------------------------------*
 *                    --- Endormir la tâche courante ---                   *
 * Entree : Neant                                                          *
 * Sortie : Neant                                                          *
 * Descrip: Endort la tâche courante et attribue le processeur à la tâche  *
 *          suivante.                                                      *
 *                                                                         *
 * Err. fatale:Neant                                                       *
 *                                                                         *
 *-------------------------------------------------------------------------*/

void  dort(void)
{
    CONTEXTE *p = &_contexte[_tache_c];
    p->status = SUSP;
    // On retire la tâche de la file des tâche en cours d'exécution.
    retire(_tache_c);
    // On fait un schedule pour passer à la tâche suivante.
    schedule();
}

/*-------------------------------------------------------------------------*
 *                    --- Réveille une tâche ---                           *
 * Entree : numéro de la tâche à réveiller                                 *
 * Sortie : Neant                                                          *
 * Descrip: Réveille une tâche. Les signaux de réveil ne sont pas mémorisés*
 *                                                                         *
 * Err. fatale:tâche non créée                                             *
 *                                                                         *
 *-------------------------------------------------------------------------*/


void reveille(uint16_t t)
{
    CONTEXTE *p = &_contexte[t]; /* acces au contexte tache */
    // Si la tâche n'existe pas, on déclenche une erreur.
    if (p->status == NCREE)
        _fatal_exception_("reveille() : p->status == NCREE");
    // On change l'état de la tâche, puis on la rajoute dans les tâches à exécuter.
    p->status = EXEC;
    ajoute(t);
    schedule();
}
