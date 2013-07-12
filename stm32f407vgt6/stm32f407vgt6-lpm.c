/*
 * stm32f407vgt6-lpm.c
 *
 *  Created on: 09.08.2012
 *      Author: pfeiffer
 */
#include "stm32f4xx.h"

#include <stdio.h>
#include <stdint.h>
#include "lpm.h"

static enum lpm_mode lpm;

enum lpm_mode lpm_set(enum lpm_mode target) {
	enum lpm_mode last_lpm = lpm;

	if( target == LPM_IDLE )
	{

	}
	else if (target == LPM_SLEEP) {
        __WFI();
	}

	lpm = target;

	return last_lpm;
}

void lpm_awake(void) {
	lpm = LPM_ON;
}
