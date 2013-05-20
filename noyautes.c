/* NOYAUTEST.C */
/*--------------------------------------------------------------------------*
 *			      Programme de tests			    *
 *--------------------------------------------------------------------------*/

#include "serialio.h"
#include "noyau.h"

/*
 ** Test du noyau preemptif. Lier noyautes.c avec noyau.c et noyaufil.c
 */

TACHE	tacheA(void);
TACHE	tacheB(void);
TACHE	tacheC(void);
TACHE	tacheD(void);

TACHE	tacheA(void)
{
  puts("------> EXEC tache A");
  active(cree(tacheB));
  active(cree(tacheC));
  active(cree(tacheD));
  fin_tache();
}

TACHE	tacheB(void)
{
  int i=0;
  long j;
  puts("------> DEBUT tache B");
  while (1) {
    for (j=0; j<30000L; j++);
    printf("======> Dans tache B %d\n",i);
    i++;
  }
}

TACHE	tacheC(void)
{
  int i=0;
  long j;
  puts("------> DEBUT tache C");
  while (1) {
    for (j=0; j<60000L; j++);
    printf("======> Dans tache C %d\n",i);
    i++;
  }
}

TACHE	tacheD(void)
{
  int i=0;
  long j;
  puts("------> DEBUT tache D");
  while (1) {
    for (j=0; j<120000L; j++);
    printf("======> Dans tache D %d\n",i++);
    if (i==50) noyau_exit();
  }
}


int main()
{
  serial_init(115200);
  puts("Test noyau");
  puts("Noyau preemptif");
  start(tacheA);
  return(0);
}

