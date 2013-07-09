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

/**
 * @addtogroup	arm_common
 * @{
 */

/**
 * @file
 * @internal
 * @brief		ARM kernel timer CPU dependent functions implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author		Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 3861 $
 *
 * @note		$Id: arm-hwtimer_arch.c 3861 2011-12-07 13:31:45Z hwill $
 *
 */
#include "cpu.h"
#include "hwtimer_cpu.h"
#include "hwtimer_arch.h"

#define SYSTICK_CONTROL (*(uint32_t*) 0xE000E010)
#define SYSTICK_RELOAD  (*(uint32_t*) 0xE000E014)
#define SYSTICK_CURRENT (*(uint32_t*) 0xE000E018)

#define SYSTICK_PASSED  (SYSTICK_RELOAD - SYSTICK_CURRENT)

#define SYSTICK_CONTROL_ENABLE      (1 << 0)
#define SYSTICK_CONTROL_TICKINT     (1 << 1)
#define SYSTICK_CONTROL_CLKSOURCE   (1 << 2)
#define SYSTICK_CONTROL_COUNTFLAG   (1 << 16)

#define ICSR    (*(uint32_t*) 0xE000ED04)
#define ICSR_PENDSTSET  (1 << 26)

int handler_enable = 0;

unsigned long tick_next_reload = 0;

unsigned long tick_remain = 0;

unsigned long tick_count;

///// High level interrupt handler
static void (*int_handler)(int);

/**
 * @brief   Adjust tick reload an remainder to fit HWTIMER_MAXTICKS
 *
 * @param[in,out] reload  Tick reload value.
 * @param[in,out] remain  Remainder ticks.
 */
void next_reload_and_remain(uint32_t *reload, uint32_t *remain) {
    if (*reload > HWTIMER_MAXTICKS) {
        /* Prevent small interrupt interval */
        if (*reload < (HWTIMER_MAXTICKS * 2)) {
            *remain = *reload / 2;
            *reload = *reload - *remain;
        }
        else {
            *remain = *reload - HWTIMER_MAXTICKS;
            *reload = HWTIMER_MAXTICKS;
        }
    }
    else {
        *remain = 0;
    }
}

/*---------------------------------------------------------------------------*/

void SysTick_Handler(void) {
    tick_count += tick_next_reload;

    if (tick_remain) {
        uint32_t offset = tick_remain;

        next_reload_and_remain(&offset, &tick_remain);

        // Subtract ticks from handler start to keep on time
        SYSTICK_RELOAD = offset - 1 - SYSTICK_PASSED;
        SYSTICK_CURRENT = 0;

        tick_next_reload = offset;
    }
    else {
        tick_next_reload = SYSTICK_RELOAD + 1;
        SYSTICK_RELOAD = HWTIMER_MAXTICKS;

        if (handler_enable) {
            handler_enable = 0;
            int_handler(0);
        }
    }
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
	int_handler = handler;

	SYSTICK_RELOAD = HWTIMER_MAXTICKS;
	SYSTICK_CONTROL = SYSTICK_CONTROL_ENABLE | SYSTICK_CONTROL_TICKINT;
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_enable_interrupt(void) {
	SYSTICK_CONTROL = SYSTICK_CONTROL_ENABLE | SYSTICK_CONTROL_TICKINT;
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_disable_interrupt(void) {
	SYSTICK_CONTROL = SYSTICK_CONTROL_ENABLE;
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set(unsigned long offset, short timer) {
    if (offset > 0) {
        handler_enable = 1;

        next_reload_and_remain(&offset, &tick_remain);

        tick_count += SYSTICK_PASSED;

        SYSTICK_RELOAD = offset - 1;
        SYSTICK_CURRENT = 0;

        tick_next_reload = offset;
    }
    else {
        tick_next_reload = offset;
        ICSR |= ICSR_PENDSTSET;
    }
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set_absolute(unsigned long value, short timer) {
    signed long offset = value - hwtimer_arch_now();
    if (offset >= 0) {
        hwtimer_arch_set(offset, timer);
    }
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_unset(short timer) {
    handler_enable = 0;
    tick_remain = 0;
}

/*---------------------------------------------------------------------------*/

/**
 * @brief   Return hardware timer absolute tick count.
 *
 * @return  Hardware timer absolute tick count.
 */
unsigned long hwtimer_arch_now(void) {
	return (SYSTICK_RELOAD - SYSTICK_CURRENT) + tick_count;
}
