/*
 * stm32f4xx_timer_0_1.h
 *
 *  Created on: 15.01.2009
 *      Author: heiko, kaspar
 *
 *      Changelog:
 *                  26.01.09 kaspar: renamed file, misc changes for firekernel
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#define ARCH_MAXTIMERS 	1
#define HWTIMER_SPEED 	(F_CPU / 8)
#define HWTIMER_MAXTICKS (0x00FFFFFF)

#endif /* HWTIMER_CPU_H_ */
