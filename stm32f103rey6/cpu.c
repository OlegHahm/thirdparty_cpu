/*
 * cpu.c
 *
 *  Created on: 07.09.2012
 *      Author: pfeiffer
 */

#include "stdint.h"
#include "stm32f10x_tim.h"

int inISR(void)
{
	return (__get_IPSR() & 0xFF);
}

unsigned int disableIRQ(void)
{
	// FIXME PRIMASK is the old CPSR (FAULTMASK ??? BASEPRI ???)
	//PRIMASK lesen
	unsigned int uiPriMask = __get_PRIMASK();
	__disable_irq();
	return uiPriMask;
}

void restoreIRQ(unsigned oldPRIMASK)
{
	//PRIMASK lesen setzen
	 __set_PRIMASK(oldPRIMASK);
}


__attribute__((naked))
void HardFault_Handler(void) {
  puts("HARD FAULT");
  while(1);
}

__attribute__((naked))
void BusFault_Handler(void) {
  puts("BusFault_Handler");
  while(1);
}

__attribute__((naked))
void Usage_Handler(void) {
  puts("Usage FAULT");
  while(1);
}

__attribute__((naked))
void WWDG_Handler(void) {
  puts("WWDG FAULT");
  while(1);
}
