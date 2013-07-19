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

#include "stm32f4xx.h"
#include "cpu.h"
#include "sched.h"
#include "hwtimer_cpu.h"
#include "hwtimer_arch.h"

static uint32_t hwtimer_arch_prescale(uint32_t fcpu, uint32_t ftimer);

///// High level interrupt handler
static void (*int_handler)(int);

void (*TIM_SetCompare[])(TIM_TypeDef *TIMx, uint32_t Compare1) = {
    TIM_SetCompare1,
    TIM_SetCompare2,
    TIM_SetCompare3,
    TIM_SetCompare4
};

uint16_t TIM_IT_CC[] = {
    TIM_IT_CC1,
    TIM_IT_CC2,
    TIM_IT_CC3,
    TIM_IT_CC4
};

uint16_t TIM_Channel_[] = {
    TIM_Channel_1,
    TIM_Channel_2,
    TIM_Channel_3,
    TIM_Channel_4
};

void TIM2_IRQHandler(void)
{
    int i;

    for (i = 0; i < ARCH_MAXTIMERS; i++) {
        if (TIM_GetITStatus(TIM2, TIM_IT_CC[i]) != RESET) {
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC[i]);
            TIM_ITConfig(TIM2, TIM_IT_CC[i], DISABLE);
            TIM_CCxCmd(TIM2, TIM_Channel_[i], TIM_CCx_Disable);

            int_handler(i);
        }
    }

    if (sched_context_switch_request) {
        __pendSV();
    }
}

static uint32_t hwtimer_arch_prescale(uint32_t fcpu, uint32_t ftimer)
{
    switch (RCC->CFGR & RCC_CFGR_HPRE) {
        case RCC_CFGR_HPRE_DIV512:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV256:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV128:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV64:
            fcpu >>= 2;

        case RCC_CFGR_HPRE_DIV16:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV8:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV4:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV2:
            fcpu >>= 1;

        case RCC_CFGR_HPRE_DIV1:
        default:
            ;
    }

    switch (RCC->CFGR & RCC_CFGR_PPRE1) {
        case RCC_CFGR_PPRE1_DIV16:
            fcpu >>= 1;

        case RCC_CFGR_PPRE1_DIV8:
            fcpu >>= 1;

        case RCC_CFGR_PPRE1_DIV4:
            fcpu >>= 1;

        case RCC_CFGR_PPRE1_DIV2:
            fcpu >>= 1;
            fcpu <<= 1; /* (APBx presc = 1) ? x1 : x2 */

        case RCC_CFGR_PPRE1_DIV1:
        default:
            ;
    }

    return fcpu / ftimer;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    uint32_t cpsr;

    int_handler = handler;
    cpsr = hwtimer_arch_prescale(fcpu, HWTIMER_SPEED);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    /* Time base configuration */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = HWTIMER_MAXTICKS; // 1 MHz down to 10 KHz (0.1 ms)
    TIM_TimeBaseStructure.TIM_Prescaler = cpsr - 1; /*1 msec reso*/ //cpsr; // Down to 1 MHz (adjust per your clock)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//not relevant in this case
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = HWTIMER_MAXTICKS;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);
    TIM_OC2Init(TIM2, &TIM_OCInitStruct);
    TIM_OC3Init(TIM2, &TIM_OCInitStruct);
    TIM_OC4Init(TIM2, &TIM_OCInitStruct);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_enable_interrupt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // Timer Interrupt konfigurieren
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_Init(&NVIC_InitStructure);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_disable_interrupt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // Timer Interrupt konfigurieren
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set(unsigned long offset, short timer)
{
    hwtimer_arch_set_absolute(offset + TIM_GetCounter(TIM2), timer);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    TIM_SetCompare[timer](TIM2, value);
    TIM_ITConfig(TIM2, TIM_IT_CC[timer], ENABLE);
    TIM_CCxCmd(TIM2, TIM_Channel_[timer], TIM_CCx_Enable);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_unset(short timer)
{
    TIM_ITConfig(TIM2, TIM_IT_CC[timer], DISABLE);
    TIM_CCxCmd(TIM2, TIM_Channel_[timer], TIM_CCx_Disable);
}

/*---------------------------------------------------------------------------*/

/**
 * @brief   Return hardware timer absolute tick count.
 *
 * @return  Hardware timer absolute tick count.
 */
unsigned long hwtimer_arch_now(void)
{
    return TIM_GetCounter(TIM2);
}
