.text
.align(4)
.code 32

/* Perform low-level board initialisations
   - Setups CPU clock
   - Maps flash to its final address
   - Setups SDRAM timings if not already executing in SDRAM
*/
.func		_lowlevel_init
.globl		_lowlevel_init
_lowlevel_init:
			/* Enable instruction cache */
			mrc		p15, 0, r0, c1, c0, 0
			orr		r0, r0, #(1<<12)
			mcr		p15, 0, r0, c1, c0, 0

			/* Setup CPU clocks */

			ldr		r0, =0x0021b000

			mov		r1, #0x15
			str 	r1, [r0, #0x20]

			/* MPLL => 192 MHz */
			ldr 	r1, =0x07b32da5
			str 	r1, [r0, #0x04]

			mov		r1,#0
			str		r1, [r0, #0x08]

			/* Reset MPLL */
			ldr 	r1, =0x0f20ac03
			str		r1, [r0]

			/* SPLL => 96 MHz */
			ldr		r1, =0x0fb32da5
			str		r1, [r0,#0x0c]

			mov		r1, #0
			str		r1, [r0,#0x10]

			ldr		r1, =0x0f40ac03
			str		r1, [r0]

			/* Wait for SPLL to stabilize */
1:			ldr		r1, [r0,#0x10]
			tst		r1, #(1<<15)
			beq		1b



			ldr		r1, =0x27000003
			str		r1, [r0]

			ldr		r1, =0x3ab
			str		r1, [r0,#0x80c]

			/* Enable PLL clock */
			mrc		p15, 0, r0, c1, c0, 0
			orr		r0, r0, #0xc0000000
			mcr		p15, 0, r0, c1, c0, 0

			/* Setup Flash: remap CS0 to 0x10000000 */
			mov		r0, #0x00220000
			ldr		r1, =0x00330e04
			str		r1, [r0]

			ldr		r1, =0xd01
			str		r1, [r0,#4]

			/* Setup SDRAM timings */

			/* Useful definitions */
.equ		PRE_ALL_CMD,		0x10000000
.equ		AUTO_REF_CMD,		0x20000000
.equ		SET_MODE_REG_CMD,	0x30000000
.equ		SDCTL_BASE_VAL,		0x810a9267
.equ		MODE_REG_VAL,		0x00223000
.equ		SBANK_BASE,			0x08000000
.equ		SDCTL,				0x00221000
.equ		SDRAM_SIZE,			16*1024*1024	@ 16M

			/* Skip SDRAM initialisation if we're running in SDRAM */
			sub		r0, pc, #SBANK_BASE
			cmp		r0, #SDRAM_SIZE
			blo		skip_sdram_setup

sdram_setup:
			ldr		r0, =SDCTL
			mov		r1, #SBANK_BASE
			ldr		r2, =(SDCTL_BASE_VAL | PRE_ALL_CMD)
			str		r2, [r0]
			ldr		r2, [r1]

			ldr		r2, =(SDCTL_BASE_VAL | AUTO_REF_CMD)
			str		r2, [r0]
			mov		r3, #8
1:			ldr		r2, [r1]
			subs	r3, r3, #1
			bne		1b

			ldr		r2, =(SDCTL_BASE_VAL | SET_MODE_REG_CMD)
			str		r2, [r0]
			ldr		r2, =MODE_REG_VAL
			orr		r1, r1, r2
			ldr		r2, [r1]
			ldr		r2, =SDCTL_BASE_VAL
			str		r2, [r0]

skip_sdram_setup:
			bx		lr
.endfunc

.end
