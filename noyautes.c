/* NOYAUTEST.C */
/*--------------------------------------------------------------------------*
 *			      Programme de tests			    *
 *--------------------------------------------------------------------------*/

#include "serialio.h"
#include "noyau.h"
#include "sem.h"

/*
 ** Test du noyau preemptif. Lier noyautes.c avec noyau.c et noyaufil.c
 */

/*
 * Les tâches.
 */
TACHE	tacheA(void);
TACHE	tacheB(void);
TACHE	tacheC(void);
TACHE	tacheD(void);
TACHE   diner(void);
TACHE   ph(void);
TACHE   ph2(void);

/*
 * Les sémaphores.
 */
static ushort dinerFinS;
static ushort diner1FinS;
static ushort dinerMangeursS;
static ushort forkSTab[5];

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
    if (i==50) {
        // Faire manger les philosophes.
        dinerFinS = s_cree(-5);
        active(cree(diner));
        // Les attendre pour fermer le restaurant.
        s_wait(dinerFinS);
        noyau_exit();
    }
  }
}

TACHE    diner(void)
{
    ushort i;
    // Initialiser les sémaphores et lancer les tâches.
    diner1FinS = s_cree(-5);
    // Il faut toujours qu'il y ait une fourchette de libre.
    dinerMangeursS = s_cree(4);
    // Une fourchette ne peut être utilisée par qu'un seul philosophe.
    for(i=0; i < 5; i++) {
        forkSTab[i] = s_cree(1);
    }
    // Mangeons gaiement - mais soyons polis, chacun son tour !
    for(i=0; i < 5; i++)
        active(cree(ph));
    s_wait(diner1FinS);
    // Détruire les sémaphores.
    s_close(dinerMangeursS);
    for(i=0; i < 5; i++) {
        s_close(forkSTab[i]);
    }
    // Relancer une seconde simulation.
    dinerMangeursS = s_cree(4);
    for(i=0; i < 5; i++) {
        forkSTab[i] = s_cree(1);
    }
    for(i=0; i < 5; i++)
        active(cree(ph2));
    fin_tache();
}

TACHE    ph(void)
{
    static ushort phI = -1;
    phI++;
    int i=0;
    long j;
    printf("------> BONJOUR, je suis le philosophe %d", phI);
    while (1) {
        // Les philosophes mangent à des intervalles différents.
        for (j=0; j < (!phI) ? 60000L : 120000L*phI; j++);
        printf("======> ph%d dit \"j'ai faim !\"\n",phI);
        s_wait(dinerMangeursS);
        s_wait(forkSTab[(phI-1)%5]);
        s_wait(forkSTab[(phI+1)%5]);
        printf("======> ph%d dit \"je commence à manger !\"\n",phI);
        for (j=0; j < (!phI) ? 60000L : 30000L*phI; j++);
        printf("======> ph%d dit \"j\'ai fini de manger !\"\n",phI);
        s_signal(dinerMangeursS);
        s_signal(forkSTab[(phI-1)%5]);
        s_signal(forkSTab[(phI+1)%5]);
        i++;
        if(phI == 1 && i == 4) {
            // YTO Mieux que ça : http://fr.wikipedia.org/wiki/Barri%C3%A8re_de_synchronisation
            s_signal(diner1FinS);
            fin_tache();
        }
    }
}

TACHE    ph2(void)
{
    static ushort phI = -1;
    phI++;
    int i=0;
    long j;
    puts("------> \nSECOND REPAS\n");
    printf("------> BONJOUR, je suis le philosophe %d", phI);
    while (1) {
        // Les philosophes mangent tous au même moment et à la même vitesse.
        for (j=0; j < 60000L; j++);
        printf("======> ph%d dit \"j'ai faim !\"\n",phI);
        s_wait(dinerMangeursS);
        s_wait(forkSTab[(phI-1)%5]);
        s_wait(forkSTab[(phI+1)%5]);
        printf("======> ph%d dit \"je commence à manger !\"\n",phI);
        for (j=0; j < 30000L; j++);
        printf("======> ph%d dit \"j\'ai fini de manger !\"\n",phI);
        s_signal(dinerMangeursS);
        s_signal(forkSTab[(phI-1)%5]);
        s_signal(forkSTab[(phI+1)%5]);
        i++;
        if(phI == 1 && i == 4) {
            s_signal(dinerFinS);
            fin_tache();
        }
    }
}


int main()
{
  serial_init(115200);
  puts("Test noyau");
  puts("Noyau preemptif");
  // Initialiser les sémaphores.
  s_init();
  start(tacheA);
  return(0);
}

