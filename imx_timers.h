/*
 * imx_timers.h
 *
 *  Created on: 25 mai 2012
 *      Author: mi03
 */

#ifndef IMX_TIMERS_H_
#define IMX_TIMERS_H_

#include <stdint.h>

#define TIMER1_BASE	((void *)(0x00202000))
#define TIMER2_BASE	((void *)(0x00203000))

struct imx_timer {
	volatile uint32_t tctl;
	volatile uint32_t tprer;
	volatile uint32_t tcmp;
	volatile uint32_t tcr;
	volatile uint32_t tcn;
	volatile uint32_t tstat;
};

/* Timers defines */
#define TCTL_SWR				(1 << 15)
#define TCTL_FRR				(1 << 8)
#define TCTL_CAP_RISING			(1 << 6)
#define TCTL_CAP_FALLING 		(2 << 6)
#define TCTL_CAP_ANY			(3 << 6)
#define TCTL_OM					(1 << 5)
#define TCTL_IRQEN				(1 << 4)
#define TCTL_CLKSOURCE_PERCLK 	(1 << 1)
#define TCTL_CLKSOURCE_PERCLK16 (2 << 1)
#define TCTL_CLKSOURCE_TIN		(3 << 1)
#define TCTL_CLKSOURCE_32KHZ	(4 << 1)
#define TCTL_TEN				(1 << 0)

#define TSTAT_CAPT				(1 << 1)
#define TSTAT_COMP				(1 << 0)

#define TIMER1_INT				59
#define TIMER2_INT				58


#endif /* IMX_TIMERS_H_ */
