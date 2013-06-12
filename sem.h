#ifndef __SEM_H__
#define __SEM_H__

/* Les constantes */
/******************/

#define MAX_SEM     8               /* Nombre maximum de sémaphores     */

/* Etat des sémaphores */
/***********************/

#define SEM_NCREE   -1         /* Etat non cree          */

/* definition d'une structure FIGO */
/***********************************/

typedef struct {
  ushort n[MAX_TACHES];       /* adresse de debut de la tache */
  ushort head;                /* tête de la file circulaire     */
  ushort queue;               /* queue de la file circulaire     */
} FIFO;

/* definition d'une structure de sémaphore */
/*******************************************/

typedef struct {
    FIFO file ; /* File circulaire des tâches en attente */
    short valeur ;/* compteur du sémaphore e(s) */
} SEMAPHORE ;

/* Variables du noyau */
/**********************/

extern SEMAPHORE _sem[MAX_SEM];  /* tableau des sémaphores      */

extern  void    s_init      ( void );
extern  ushort  s_cree      ( short v );
extern  void    s_close     ( ushort n );
extern  void    s_wait      ( ushort n );
extern  void    s_signal    ( ushort n );

#endif