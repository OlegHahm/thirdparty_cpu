/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#ifndef __CPU_H
#define __CPU_H

/**
 * @defgroup	
 * @ingroup		cpu
 * @{
 */

#include <stdbool.h>
#include "stm32f407vgt6.h"

void thread_yield(void);
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t* prescale);

__attribute__( ( always_inline ) ) static __INLINE int inISR(void)
{
	return (__get_IPSR() & 0xFF);
}

__attribute__( ( always_inline ) ) static __INLINE unsigned int disableIRQ(void)
{
	// FIXME PRIMASK is the old CPSR (FAULTMASK ??? BASEPRI ???)
	//PRIMASK lesen
	unsigned int uiPriMask = __get_PRIMASK();
	__disable_irq();
	return uiPriMask;
}

__attribute__( ( always_inline ) ) static __INLINE void restoreIRQ(unsigned oldPRIMASK)
{
	//PRIMASK lesen setzen
	 __set_PRIMASK(oldPRIMASK);
}

__attribute__( ( always_inline ) ) static __INLINE void dINT(void)
{
	__disable_irq();
}

__attribute__( ( always_inline ) ) static __INLINE void eINT(void)
{
	__enable_irq();
}


__attribute__( ( always_inline ) ) static __INLINE void save_context(void)
{
	/* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
	asm("push 	{r4-r11}");
	/* save unsaved registers */
	/*vstmdb	sp!, {s16-s31}	*/ //FIXME save fpu registers if needed
	asm("push 	{LR}");
	/* save exception return value */

	asm("ldr     r1, =active_thread");
	/* load address of currend pdc */
	asm("ldr     r1, [r1]");
	/* deref pdc */
	asm("str     sp, [r1]");
	/* write sp to pdc->sp means current threads stack pointer */
}

__attribute__( ( always_inline ) ) static __INLINE void restore_context(void)
{
	asm("ldr     r0, =active_thread");
	/* load address of currend pdc */
	asm("ldr     r0, [r0]");
	/* deref pdc */
	asm("ldr     sp, [r0]");
	/* load pdc->sp to sp register */

	asm("pop		{r0}");
	/* restore exception retrun value from stack */
	/*pop		{s16-s31}		*/ //FIXME load fpu register if needed depends on r0 exret
	asm("pop		{r4-r11}");
	/* load unloaded register */
//	asm("pop 		{r4}"); /*foo*/
	asm("bx		r0");				/* load exception return value to pc causes end of exception*/
							/* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}


//extern uintptr_t __stack_start;		///< end of user stack memory space

/*
#define SYSMON_STACKPTR		({ register void* __stack_ptr;				\
		__asm__ __volatile__ ( "mov %0, sp" : "=r" (__stack_ptr) );		\
		__stack_ptr; })
#define SYSMON_STACKSTART	(&__stack_start)
*/

//void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t* pclksel, uint32_t* prescale);
//bool install_irq( int IntNumber, void *HandlerAddr, int Priority );

/** @} */
#endif /* __CPU_H */
