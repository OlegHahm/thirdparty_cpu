/*
 * cpu.c
 *
 *  Created on: 07.09.2012
 *      Author: pfeiffer
 */

#include "stdint.h"
#include "stm32f4xx_tim.h"

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

void dINT(void)
{
	__disable_irq();
}

void eINT(void)
{
	__enable_irq();
}

void stm32f4_pclk_scale(uint32_t source, uint32_t target, uint32_t* pclksel, uint32_t* prescale)
{
    uint32_t pclkdiv;
    *prescale = source / target;

    if ((*prescale % 4) == 0) {
        *pclksel = TIM_CKD_DIV4;
        pclkdiv = 4;
    }
    else if ((*prescale % 2) == 0) {
        *pclksel = TIM_CKD_DIV2;
        pclkdiv = 2;
    }
    else {
        *pclksel = TIM_CKD_DIV1;
        pclkdiv = 1;
    }

    *prescale /= pclkdiv;

    if (*prescale % 2) {
        (*prescale)++;
    }
}


void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale)
{
    uint32_t pclksel;

    stm32f4_pclk_scale(source, target, &pclksel, prescale);

    //PCLKSEL0 = (PCLKSEL0 & ~(BIT2|BIT3)) | (pclksel << 2); 		// timer 0
    //PCLKSEL0 = (PCLKSEL0 & ~(BIT4|BIT5)) | (pclksel << 4); 		// timer 1
    //PCLKSEL1 = (PCLKSEL1 & ~(BIT12|BIT13)) | (pclksel << 12);	// timer 2
}
