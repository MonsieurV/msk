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
  	/* Désactiver les interruptions */
    printf("Sortie du noyau\n");
	/* afficher par exemple le nombre d'activation de chaque tache */
								
	/* Terminer l'exécution */
	
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
	
  /* la tache est enlevee de la file des taches */

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
									/* numero de tache suivant */

  if (tache >= MAX_TACHES)        /* sortie si depassement */
	{
				/* sortie car depassement       */
	}	

									/* contexte de la nouvelle tache */

									/* allocation d'une pile a la tache */
									/* decrementation du pointeur de pile pour la prochaine tache. */

									/* fin section critique */

									/* memorisation adresse debut de tache */
									/* mise a l'etat CREE */
  return(tache);                  /* tache est un uint16_t */
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
  {
				/* sortie du noyau         		 */
  }

									/* debut section critique */
  if (p->status == CREE)          	/* n'active que si receptif */
  {
									/* changement d'etat, mise a l'etat PRET */
									/* ajouter la tache dans la liste */
									/* activation d'une tache prete */
  }
									/* fin section critique */
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
									/* Attendre un cycle */
									/* Ajustement pointeur de pile */
									/* Sauvegarde de spsr_irq */
						);			/* et de lr_irq */

  if (_ack_timer)                 /* Réinitialiser le timer si nécessaire */
  {
    							/* Acquiter l'événement de comparaison du Timer pour pouvoir */
								/* obtenir le déclencement d'une prochaine interruption */
  }
  else
  {
    _ack_timer = 1;
  }

									/* memoriser le pointeur de pile */
									/* recherche du suivant */
	 
									 
									/* Incrémenter le compteur d'activations  */
									/* p pointe sur la nouvelle tache courante*/

  if (p->status == PRET)          /* tache prete ? */
  {
									/* Charger sp_irq initial */
									/* Passer en mode système */
									/* Charger sp_sys initial */
									/* status tache -> execution */
									/* autoriser les interuptions   */
									/* lancement de la tâche */
  }
  else
  {
									/* tache deja en execution, restaurer sp_irq */
  }

  /* Restaurer le contexte complet depuis la pile IRQ */
  __asm__ __volatile__(
									/* Restaurer lr_irq */
									/* et spsr_irq */
									/* Restaurer registres mode system */
									/* Attendre un cycle */
									/* Ajuster pointeur de pile irq */
					  );   			/* Retour d'exception */
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

    /* On simule une exception irq pour forcer un appel correct à scheduler().*/
	/* Passer en mode IRQ */
    _set_arm_mode_(ARMMODE_IRQ);
  __asm__ __volatile__(
		/* Sauvegarder cpsr dans spsr */
        "mrs  r0, cpsr\t\n"
		"msr  spsr, r0\t\n"
        
		/* Sauvegarder pc dans lr et l'ajuster */
        "mov  lr, pc\t\n"
        /* YTO: on ne veut pas que ce soit l'instruction courante qui soit
         * exécutée en retour de scheduler(), ni non plus la suivante, 
         * mais celle d'après.
         * Quelle est la valeur de pc lors de l'instruction " mov  lr, pc" ?
         * D'après http://stackoverflow.com/questions/2102921/strange-behaviour-of-ldr-pc-value
         * la valeur est celle de l'instruction suivante (la courante + 4 bytes).
         * Il faut donc encore décaler de 8 bytes pour obtenir l'instruction à n+2. */
        "add  lr, #8\t\n"
        
	    /* Saut à scheduler */
        /* YTO: Faire exécuter le vecteur d'exception IRQ
         * Adresse du vecteur : 0x00000018 */
        "mov  pc, #0x18\t\n"
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
  }
									/* initialisation de la tache courante */
									/* initialisation de la file           */

									/* Initialisation de la variable Haut de la pile des tâches */
									/* Passer en mode IRQ */
									/* sp_irq initial */
									/* Repasser en mode SYS */

									/* on interdit les interruptions */

  /* Initialisation du timer à 100 Hz */

  
  /* Initialisation de l'AITC */
  

									/* creation et activation premiere tache */
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

}

