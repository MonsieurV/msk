@ Créer la section init (ax pour accessible et executable).
.section	".init",	"ax", %progbits
.code 32
.align 4
vector:
		@ Table des vecteurs d'exception
		b  start     @ Reset vector
		b  .         @ Undefined instruction
		b  .         @ Software interrupt
		b  .         @ Prefetch abort exception
		b  .         @ Data abort exception
		b  .         @ Reserved vector, not used
		b  scheduler @ IRQ YTO: On branche sur la fonction scheduler() de noyau.c
		b  .         @ Fast interrupt
start:
		@ Exécution des instructions bas level pour initialiser le matériel.
		bl  _lowlevel_init

        @ Initialisation à 0 de la section BSS.
		ldr  r0, =BSS_START
		ldr  r1, =BSS_END
		ldr  r2, =0
lp1_check:
		cmp  r1,  r0
		@ Sortie de la boucle par saut conditionnel si r0 >= r1.
		ble  lp1_end
		@ Met un 0 à l'adresse r0 de la mémoire
		@ str, par défaut, copie 4 octets par 4 octets.
		@ Cela est valide, car nos sections sont alignées sur des multiples de 4.
		@ Il ne faut pas oublier d'incrémenter r0 de 4 octets (#4).
		str  r2, [r0], #4
		b  lp1_check
lp1_end:
        @ Chargement de la section DATA en mémoire vive à partir des données en flash.
        ldr  r0, =DATA_RAM_START
        ldr  r1, =DATA_FLASH_START
        ldr  r2, =DATA_RAM_END
lp2_check:
    	cmp  r2,  r0
		ble  lp2_end
        ldr  r3, [r1], #4
		str  r3, [r0], #4
		b  lp2_check
lp2_end:
        @ Passer en mode SYS.
        mrs  r0, cpsr
        bic  r0, r0, #0x1f
        orr  r0, r0, #0x1f
        msr  cpsr_c, r0
        nop
		@ Initialiser le pointeur de pile SYS.
		ldr  r0, =MEMORY_END
		mov  sp, r0
		@ Appel au main() du C.
		bl  main
        @ Sortie du main : on ne fait rien, on arrête l'exécution.
.func    	_main_exit
.globl		_main_exit
_main_exit:
        b  .
.end
