/* NOYAU.H */
/*--------------------------------------------------------------------------*
 *  mini noyau temps reel fonctionnant sur MC9328MXL                        *
 *		    NOYAU.H                                                 *
 ****************************************************************************
 * On definit dans ce fichier toutes les constantes et les structures       *
 * necessaires au fonctionnement du noyau
 *--------------------------------------------------------------------------*/

#ifndef __NOYAU_H__
#define __NOYAU_H__

#include <stdint.h>

/* Les constantes */
/******************/

#define PILE_TACHE  2048            /* Taille maxi de la pile d'une tâche   */
#define PILE_IRQ    2048            /* Taille maxi de la pile IRQ par tâche */
#define MAX_TACHES  8               /* Nombre maximum de taches     */
#define F_VIDE      MAX_TACHES      /* numero de tache hors tableau */


/*  Definitions des fonctions dependant du materiel sous forme
 *  de code inline.
 ***************************************************************/

#define _lock_() __asm__ __volatile__(\
		"mrs	r0, cpsr\t\n"\
		"stmfd  sp!,{r0}\t\n"\
		"orr	  r0, #0x80\t\n"\
		"msr    cpsr_c, r0\t\n"\
		:::"r0")

#define _unlock_() __asm__ __volatile__(\
		"ldmfd  sp!,{r0}\t\n"\
		"msr    cpsr_c, r0\t\n"\
		:::"r0")

#define _irq_enable_() \
  __asm__ __volatile__ (\
      "mrs  r0,cpsr\t\n" \
      "bic  r0,r0,#0x80\t\n" \
      "msr  cpsr_c,r0\t\n"\
      :\
      :\
      :"r0")

#define _irq_disable_() \
  __asm__ __volatile__ (\
      "mrs  r0,cpsr\t\n" \
      "orr  r0,r0,#0x80\t\n" \
      "msr  cpsr_c,r0\t\n"\
      :\
      :\
      :"r0")

#define _set_arm_mode_(mode) \
  __asm__ __volatile__(\
      "mrs  r0, cpsr\t\n"\
      "bic  r0, r0, #0x1f\t\n"\
      "orr  r0, r0, %0\t\n"\
      "msr  cpsr_c, r0\t\n"\
      "nop\t\n"\
      :\
      : "I" (mode)\
      : "r0")

#define ARMMODE_USR   0x10
#define ARMMODE_SYS   0x1f
#define ARMMODE_SVC   0x13
#define ARMMODE_ABT   0x17
#define ARMMODE_UND   0x1b
#define ARMMODE_IRQ   0x12
#define ARMMODE_FIQ   0x11

/* Etat des taches */
/*******************/

#define NCREE   0         /* Etat non cree          */
#define CREE    0x8000    /* Etat cree ou dormant   */
#define PRET    0x9000    /* Etat eligible          */
#define SUSP    0xA000    /* Etat suspendu          */
#define EXEC    0xC000    /* Etat execution         */

/* definition des types */
/************************/

#define TACHE   void
typedef TACHE   (*TACHE_ADR) (void); /* pointeur de taches	*/
#define ushort		unsigned short
#define uchar		unsigned char

/* definition du contexte d'interruption d'une tache */
/*****************************************************/

/* Structure du contexte sur la pile IRQ */
typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r12;
  uint32_t sp;
  uint32_t lr;
  uint32_t lr_irq;
  uint32_t spsr_irq;
} REGS;

/* definition du contexte d'une tache */
/**************************************/

typedef struct {
  TACHE_ADR tache_adr;       /* adresse de debut de la tache */
  uint16_t  status;          /* etat courant de la tache     */
  uint32_t  sp_ini;          /* valeur initiale de SP        */
  uint32_t  sp_irq;          /* valeur courante de SP        */
} CONTEXTE;

/* Variables du noyau */
/**********************/

extern CONTEXTE _contexte[MAX_TACHES];  /* tableau des contextes      */
extern volatile uint16_t _tache_c;      /* numero de tache courante 	*/
extern uint32_t   _tos;                 /* adresse du sommet de pile	*/

/* Prototype des fonctions */
/***************************/

extern  void      file_init   ( void );
extern  void      ajoute      ( uint16_t n );
extern  void      retire      ( uint16_t t );
extern  uint16_t  suivant     ( void );
extern  void      affic_queue ( void );
extern  void      affic_file  ( void );
extern  void      noyau_exit  ( void );
extern  void      fin_tache   ( void );
extern  uint16_t  cree        ( TACHE_ADR adr_tache );
extern  void      active      ( uint16_t tache );
extern  void      schedule    ( void );
extern  void      start       ( TACHE_ADR adr_tache );
extern  void      dort        ( void );
extern  void      reveille    ( uint16_t tache );


#endif


