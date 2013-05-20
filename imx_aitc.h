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

#ifndef IMX_AITC_H_
#define IMX_AITC_H_

#include <stdint.h>

/* AITC base address */
#define AITC_BASE     0x00223000

/* AITC register bits */
#define INTCTL_FIAD   (1 << 19)
#define INTCTL_NIAD   (1 << 20)

struct imx_aitc {
  volatile uint32_t   intctl;
  volatile uint32_t   nimask;
  volatile uint32_t   intennum;
  volatile uint32_t   intdisnum;
  volatile uint32_t   intenableh;
  volatile uint32_t   intenablel;
  volatile uint32_t   inttypeh;
  volatile uint32_t   inttypel;
  volatile uint32_t   nipriority[8];
  volatile uint32_t   nivecsr;
  volatile uint32_t   fivecsr;
  volatile uint32_t   intsrch;
  volatile uint32_t   intsrcl;
  volatile uint32_t   intfrch;
  volatile uint32_t   intfrcl;
  volatile uint32_t   nipndh;
  volatile uint32_t   nipndl;
  volatile uint32_t   fipndh;
  volatile uint32_t   fipndl;
};

#endif /* IMX_AITC_H_ */
