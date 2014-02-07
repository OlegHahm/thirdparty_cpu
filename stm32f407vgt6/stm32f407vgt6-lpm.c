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
 * @file    stm32f407vgt6-lpm.c
 * @author  Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include "lpm.h"

/* lpm is accessed before memory init and initialized separately through code */
__attribute__((section(".noinit")))
static enum lpm_mode lpm;

/* FIXME IMPL SLEEP MODES */
enum lpm_mode lpm_set(enum lpm_mode target) {
    enum lpm_mode last_lpm = lpm;

    if (target == LPM_IDLE) {

    }

    /* calculate target mcu power mode */
    lpm = target;

    return last_lpm;
}

void lpm_awake(void)
{
#if LPM_DEBUG
    unsigned long usec = RTC_CTC;
#endif
    lpm = LPM_ON;
}
