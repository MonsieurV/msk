void init_config () {
	// INITIALISATION DU CONTROLEUR D'EXCEPTION (p.134)
	// INTENABLE H/L = 1 : actif, 0 : inactif
	int*  intenableh = (int*)0x00223010;
	int*  intenablel = (int*)0x00223014;
	// INTTYPE H/L = 0 : IRQ, 1 : FIQ
	int* inttypeh = (int*)0x00223018;
	int* inttypel = (int*)0x0022301C;
	// NIMASK = niveau minimal de priorité (priorité de 0-16)
	int* nimask = (int*)0x00223004;
	// NIPRIORITY7
	int* nipriority7 = (int*)0x00223020;
	// "Writing all 1's [...] does not disable any normal interrupt priority levels"
	*nimask = 0x1F;
	*inttypeh = 0;
	*inttypel = 0;
	* nipriority7 = 0xF000;
	// Activer la gestion des exceptions.
	*intenableh = 0x08000000;
	*intenablel = 0;

	// INITIALISATION TIMER (p. 700)
	// Port number.
	// int i = 31;
	// Port D BA (base address).
	int *ba = (int*)0x0021C300;
	// Register GIUS_D. +0x20
	int *gius = ba + (32 / sizeof(int));
	// int *ocr1 = ba + 0x00000004;
	// OCR2_D +0x8
	int *ocr2 = ba + 0x00000008 / sizeof(int);
	// DR_D + 0x1C
	int *dr = ba + 0x0000001C / sizeof(int);
	int *ddir = ba;
	/**
	 * Attention : on utilise des int, donc lorsque l'on décale,
	 * le décalage se fait en fonction de la taille d'un int.
	 * Il faut donc corriger le décalage : / sizeof(int).
	 */
	// 1. Set bit[i] in GIUS.
	// Bit 31
	*gius = *gius | 0x80000000;
	// 3.1 Set bits [2i-32+1] and [2i-32] in the Port D Output Configuration Register 2 (OCR2_D).
	// Bits 30 et 31
	*ocr2 = *ocr2 | 0xC0000000;
	//3.3 Set bit[i] in the Port D Data Direction Register (DDIR_D)
	// Bit 31
	*ddir = *ddir | 0x80000000;
	int *tctcl1 = (int*)0x00202000;
	int *tprer1 = (int*)0x00202004;
	int *tcmp1 = (int*)0x00202008;
	/**
	 * Configuration TIMER
	 * TCTCL1 : SWR = 0 - FRR = 0 - CAP = 00 - OM = 0 - IRQEN = 1 - CLKSOURCE = 010 - TEN = 1
	 * TPRER : 0X64 = 100 decimal
	 * TCMP1 : 0X2710 = 10000 decimal
	 */
	*tctcl1 = 0x00000015;
	*tprer1 = 0x00000064;
	*tcmp1 = 0x00002710;
	// 3.2 Write desired output value to bit[i] of the Port D Data Register (DR_D)
	// Bit 31
	*dr = 0x00000000;
}