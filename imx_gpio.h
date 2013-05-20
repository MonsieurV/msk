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

#ifndef IMX_GPIO_H_
#define IMX_GPIO_H_

/* Registres GPIO */
struct imx_gpio {
  volatile uint32_t ddir;
  volatile uint32_t ocr1;
  volatile uint32_t ocr2;
  volatile uint32_t iconfa1;
  volatile uint32_t iconfa2;
  volatile uint32_t iconfb1;
  volatile uint32_t iconfb2;
  volatile uint32_t dr;
  volatile uint32_t gius;
  volatile uint32_t ssr;
  volatile uint32_t icr1;
  volatile uint32_t icr2;
  volatile uint32_t imr;
  volatile uint32_t isr;
  volatile uint32_t gpr;
  volatile uint32_t swr;
  volatile uint32_t puen;
};

/* Adresses des ports */
#define PORTA_BASE ((void *)(0x0021c000))
#define PORTB_BASE ((void *)(0x0021c100))
#define PORTC_BASE ((void *)(0x0021c200))
#define	PORTD_BASE ((void *)(0x0021c300))

#endif /* IMX_GPIO_H_ */
