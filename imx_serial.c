/*
 Copyright 2012 Stéphane Bonnet (bonnetst@utc.fr)

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "imx_serial.h"
#include "imx_gpio.h"

/*  Registres UART */
struct imx_serial {
  volatile uint32_t urxd[16];
  volatile uint32_t utxd[16];
  volatile uint32_t ucr1;
  volatile uint32_t ucr2;
  volatile uint32_t ucr3;
  volatile uint32_t ucr4;
  volatile uint32_t ufcr;
  volatile uint32_t usr1;
  volatile uint32_t usr2;
  volatile uint32_t uesc;
  volatile uint32_t utim;
  volatile uint32_t ubir;
  volatile uint32_t ubmr;
  volatile uint32_t ubrc;
  volatile uint32_t bipr[4];
  volatile uint32_t bmpr[4];
  volatile uint32_t uts;
};

/* Adresse de base de l'UART 1 */
#define UART1_BASE  ((void *)(0x00206000))

/* Masques */
#define URXD_CHARRDY    (1 << 15)
#define URXD_ERR        (1 << 14)
#define URXD_OVRRUN     (1 << 13)
#define URXD_FRMERR     (1 << 12)
#define URXD_BRK        (1 << 11)
#define URXD_PRERR      (1 << 10)

#define UCR1_ADEN       (1 << 15)
#define UCR1_ADBR       (1 << 14)
#define UCR1_TRDYEN     (1 << 13)
#define UCR1_IDEN       (1 << 12)
#define UCR1_ICD4       (0 << 10)
#define UCR1_ICD8       (1 << 10)
#define UCR1_ICD16      (2 << 10)
#define UCR1_ICD32      (3 << 10)
#define UCR1_RRDYEN     (1 << 9)
#define UCR1_RDMAEN     (1 << 8)
#define UCR1_IREN       (1 << 7)
#define UCR1_TXMPTYEN   (1 << 6)
#define UCR1_RTSDEN     (1 << 5)
#define UCR1_SNDBRK     (1 << 4)
#define UCR1_TDMAEN     (1 << 3)
#define UCR1_UARTCLKEN  (1 << 2)
#define UCR1_DOZE       (1 << 1)
#define UCR1_UARTEN     (1 << 0)

#define UCR2_ESCI       (1 << 15)
#define UCR2_IRTS       (1 << 14)
#define UCR2_CTSC       (1 << 13)
#define UCR2_CTS        (1 << 12)
#define UCR2_ESCEN      (1 << 11)
#define UCR2_RTEC_RISING (0 << 9)
#define UCR2_RTEC_FALLING (1 << 9)
#define UCR2_RTEC_ANY   (1 << 10)
#define UCR2_PREN       (1 << 8)
#define UCR2_PROE       (1 << 7)
#define UCR2_STPB       (1 << 6)
#define UCR2_WS         (1 << 5)
#define UCR2_RTSEN      (1 << 4)
#define UCR2_TXEN       (1 << 2)
#define UCR2_RXEN       (1 << 1)
#define UCR2_SRST       (1 << 0)

#define UCR3_DPEC_RISING  (0 << 14)
#define UCR3_DPEC_FALLING (1 << 14)
#define UCR3_DPEC_ANY   (1 << 15)
#define UCR3_DTREN      (1 << 13)
#define UCR3_PARERREN   (1 << 12)
#define UCR3_FRAERREN   (1 << 11)
#define UCR3_DSR        (1 << 10)
#define UCR3_DCD        (1 << 9)
#define UCR3_RI         (1 << 8)
#define UCR3_RXDSEN     (1 << 6)
#define UCR3_AIRINTEN   (1 << 5)
#define UCR3_AWAKEN     (1 << 4)
#define UCR3_REF25      (1 << 3)
#define UCR3_REF30      (1 << 2)
#define UCR3_INVT       (1 << 1)
#define UCR3_BPEN       (1 << 0)

#define UCR4_CTSTL(n)   ((n & 0x3f) << 10)
#define UCR4_INVR_LOW   (0 << 9)
#define UCR4_INVR_HIGH  (1 << 9)
#define UCR4_ENIRI      (1 << 8)
#define UCR4_WKEN       (1 << 7)

#define UCR4_REF16      (1 << 6)
#define UCR4_IRSC       (1 << 5)
#define UCR4_TCEN       (1 << 3)
#define UCR4_BKEN       (1 << 2)
#define UCR4_OREN       (1 << 1)
#define	UCR4_DREN       (1 << 0)

#define UFCR_TXTL(n)    ((n & 0x3f) << 10)
#define UFCR_RFDIV1     (5 << 7)
#define UFCR_RFDIV2     (4 << 7)
#define UFCR_RFDIV3     (3 << 7)
#define UFCR_RFDIV4     (2 << 7)
#define UFCR_RFDIV5     (1 line << 7)
#define UFCR_RFDIV6     (0 << 7)
#define UFCR_RFDIV7     (6 << 7)
#define UFCR_RXTL(n)    ((n & 0x3f) << 0)

#define USR1_PARITYERR  (1 << 15)
#define USR1_RTSS       (1 << 14)
#define USR1_TRDY       (1 << 13)
#define USR1_RTSD       (1 << 12)
#define USR1_ESCF       (1 << 11)
#define USR1_FRAMERR    (1 << 10)
#define USR1_RRDY       (1 << 9)
#define USR1_RXDS       (1 << 6)
#define USR1_AIRINT     (1 << 5)
#define USR1_AWAKE      (1 << 4)

#define USR2_ADET       (1 << 15)
#define USR2_TXFE       (1 << 14)
#define USR2_DTRF       (1 << 13)
#define USR2_IDLE       (1 << 12)

#define USR2_IRINT      (1 << 8)
#define USR2_WAKE       (1 << 7)
#define USR2_RTSF       (1 << 4)
#define USR2_TXDC       (1 << 3)
#define USR2_BRCD       (1 << 2)
#define USR2_ORE        (1 << 1)
#define USR2_RDR        (1 << 0)

#define UTS_FRCPERR     (1 << 13)
#define UTS_LOOP        (1 << 12)
#define UTS_TXEMPTY     (1 << 6)
#define UTS_RXEMPTY     (1 << 5)
#define UTS_TXFULL      (1 << 4)
#define UTS_RXFULL      (1 << 3)
#define UTS_SOFTRST     (1 << 0)

/* Initialise le port série à la vitesse passée en paramètre (bits/s),
 * 8 bits de données, pas de parité, 1 bit de stop
 */
void serial_init(int baudrate) {
  volatile struct imx_serial* base = (struct imx_serial *) UART1_BASE;
  volatile struct imx_gpio* portc = (struct imx_gpio *) PORTC_BASE;

  /* Utiliser le port C pour la liaison série */
  portc->gius &= ~(3 << 11);
  portc->gpr &= ~(3 << 11);

  /* Désactiver l'UART */
  base->ucr1 &= ~UCR1_UARTEN;
  base->ucr2 &= ~UCR2_SRST;
  while (!(base->ucr2 & UCR2_SRST)) continue;

  /* Initialiser la source d'horloge */
  base->ucr4 |= UCR4_REF16;

  /* Configurer les FIFO */
  base->ufcr = UFCR_RXTL(0) | UFCR_TXTL(32) | UFCR_RFDIV1;

  /* Numérateur du diviseur de fréquence */
  base->ubir = (baudrate / 100) - 1;

  /* Dénominateur du diviseur le fréquence */
  base->ubmr = 10000 - 1;

  /* Format 8N1 */
  base->ucr2 &= ~UCR2_PREN;
  base->ucr2 |= UCR2_WS;
  base->ucr2 &= ~UCR2_STPB;

  /* Ignorer RTS */
  base->ucr2 |= UCR2_IRTS;

  /* Activer l'UART */
  base->ucr1 |= UCR1_UARTEN | UCR1_UARTCLKEN;

  /* Activer les FIFO d'emission et de réception */
  base->ucr2 |= UCR2_RXEN | UCR2_TXEN;
}

/* serial_outbyte
 * Emet l'octet passé en entrée */
void serial_outbyte(uint8_t b) {
  volatile struct imx_serial* base = (struct imx_serial *) UART1_BASE;

  while (!(base->usr1 & USR1_TRDY)) continue;
  base->utxd[0] = b;

}

/* serial_getchar
 * Retourne un caractère lu sur le port série 1. Bloque si aucun caractère
 * n'a été reçu.
 */
uint8_t serial_inbyte(void) {
  volatile struct imx_serial* base = (struct imx_serial *) UART1_BASE;

  while (!(base->usr1 & USR1_RRDY)) continue;
  return (base->urxd[0] & 0xff);
}
