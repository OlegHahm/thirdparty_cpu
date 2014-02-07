/**
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup stm32f407vgt6
 * @{
 * @file    atom.c
 * @author  Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @}
 */

#include "sched.h"
#include "cpu.h"

extern void sched_task_exit(void);
void sched_task_return(void);

unsigned int atomic_set_return(unsigned int *p, unsigned int uiVal)
{
    //unsigned int cspr = disableIRQ();		//crashes
    dINT();
    unsigned int uiOldVal = *p;
    *p = uiVal;
    //restoreIRQ(cspr);						//crashes
    eINT();
    return uiOldVal;
}
