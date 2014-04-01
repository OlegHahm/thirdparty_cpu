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
#include <string.h>
#include "stm32f103rey6.h"

#define GET_CPU_ID(id)          memcpy(&id, (void *)(0x1ffff7e8), CPU_ID_LEN)

typedef enum {
    HARD_FAULT,
    WATCHDOG
} panic_t;

void thread_yield(void);
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

int inISR(void);

unsigned int disableIRQ(void);

void restoreIRQ(unsigned oldPRIMASK);

__attribute__((always_inline)) __INLINE static void dINT(void)
{
    __disable_irq();
}

__attribute__((always_inline)) __INLINE static void eINT(void)
{
    __enable_irq();
}

__attribute__((always_inline)) __INLINE void save_context(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */

    /* save unsaved registers */
    asm("mrs    r0, psp"            );      // get stack pointer from user mode
    asm("stmdb  r0!,{r4-r11}"       );      // save regs
    asm("stmdb  r0!,{lr}"           );      // exception return value
//  asm("vstmdb sp!, {s16-s31}"     );      // FIXME save fpu registers if needed
    /* load address of currend pdc */
    asm("ldr    r1, =active_thread" );      /* load address of currend tcb */
    /* deref pdc */
    asm("ldr    r1, [r1]"           );      /* deref pdc */
    /* write r0 to pdc->sp means current threads stack pointer */
    asm("str    r0, [r1]"           );      /* write r0 to pdc->sp means current threads stack pointer */
}

__attribute__((always_inline)) __INLINE void restore_context(void)
{
    asm("ldr    r0, =active_thread" );      /* load address of currend tcb */
    asm("ldr    r0, [r0]"           );      /* deref tcb */
    asm("ldr    r1, [r0]"           );      /* load tcb->sp to register 1 */
    asm("ldmia  r1!, {r0}"          );      /* restore exception retrun value from stack */
//  asm("pop    {s16-s31}"          );      /* FIXME load fpu register if needed depends on r0 exret */
    asm("ldmia  r1!, {r4-r11}"      );      /* restore other registers */
    asm("msr    psp, r1"            );      /* restore PSP register (user mode sp)*/
    asm("bx     r0"                 );      /* load exception return value to pc causes end of exception*/
}

__attribute__( ( always_inline ) ) static __INLINE void interrupt_entry(void)
{
	/* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
	asm("push 	{LR}");
	/* save exception return value */
}

__attribute__( ( always_inline ) ) static __INLINE void interrupt_return(void)
{
	asm("pop		{r0}");
	/* restore exception retrun value from stack */
	asm("bx		r0"); /* load exception return value to pc causes end of exception*/
	/* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

/** @} */
#endif /* __CPU_H */
