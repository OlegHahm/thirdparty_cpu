#include <stdio.h>
#include <stdint.h>
#include "lpm.h"

/* lpm is accessed before memory init and initialized separately through code */
__attribute__((section(".noinit")))
static enum lpm_mode lpm;

// TODO
enum lpm_mode lpm_set(enum lpm_mode target) {
	enum lpm_mode last_lpm = lpm;

	lpm = target;

	return last_lpm;
}

void lpm_awake(void) {
	lpm = LPM_ON;
}
