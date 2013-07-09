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
//#include "bitarithm.h"
#include "hwtimer_cpu.h"
#include "hwtimer_arch.h"
//#include "arm_common.h"

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

//#define VULP(x) ((volatile unsigned long*) (x))
//
///// High level interrupt handler
static void (*int_handler)(int);

//Only kept for testing purposes
//void TIM3_IRQHandler(void) {
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
//	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1 )) {
//		GPIO_WriteBit(GPIOB, GPIO_Pin_1, RESET);
//		puts("i");
//	} else {
//		GPIO_WriteBit(GPIOB, GPIO_Pin_1, SET);
//		puts("a");
////		USART3_SendString("Test");
//	}
//}

#include "sched.h"

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

__attribute__((naked))
void TIM3_IRQHandler(void) {
	save_context();

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
//	TIM_Cmd(TIM3, DISABLE);
	if(!(TIM3->CR1 & TIM_CR1_CEN)){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
		int_handler(0);
	}

	if(sched_context_switch_request)
		{
			// scheduler
			sched_run();
		}

	restore_context();
}

__attribute__((naked))
void TIM4_IRQHandler(void) {
	save_context();

	TIM_ClearITPendingBit(TIM4, TIM_IT_Update );
//	TIM_Cmd(TIM3, DISABLE);
	if(!(TIM4->CR1 & TIM_CR1_CEN)){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
		int_handler(1);
	}

	if(sched_context_switch_request)
	{
		// scheduler
		sched_run();
	}

	restore_context();
}

__attribute__((naked))
void TIM6_DAC_IRQHandler(void) {
	save_context();

	TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
//	TIM_Cmd(TIM3, DISABLE);
	if(!(TIM6->CR1 & TIM_CR1_CEN)){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
		int_handler(2);
	}

	if(sched_context_switch_request)
	{
		// scheduler
		sched_run();
	}

	restore_context();
}

__attribute__((naked))
void TIM7_IRQHandler(void) {
	save_context();

	TIM_ClearITPendingBit(TIM7, TIM_IT_Update );
//	TIM_Cmd(TIM3, DISABLE);
	if(!(TIM7->CR1 & TIM_CR1_CEN)){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
		int_handler(3);
	}

	if(sched_context_switch_request)
	{
		// scheduler
		sched_run();
	}

	restore_context();
}


//
///// Timer 0-3 interrupt handler
//static void timer_irq(void) __attribute__((interrupt("IRQ")));
//
//inline unsigned long get_base_address(short timer) {
//	return (volatile unsigned long)(TMR0_BASE_ADDR + (timer / 8) * 0x6C000 + (timer/4 - (timer/8)*2) * 0x4000);
//}
//
//static void timer_irq(void)
//{
//	short timer = 0;
//	if (T0IR) {
//		timer = 0;
//	} else if (T1IR) {
//		timer = 4;
//	} else if (T2IR) {
//		timer = 8;
//	}
//
//	volatile unsigned long base = get_base_address(timer);
//
//    if (*VULP(base+TXIR) & BIT0) {
//    	*VULP(base+TXMCR) &= ~BIT0;
//    	*VULP(base+TXIR) = BIT0;
//		int_handler(timer);
//	}
//	if (*VULP(base+TXIR) & BIT1) {
//		*VULP(base+TXMCR) &= ~BIT3;
//		*VULP(base+TXIR) = BIT1;
//		int_handler(timer + 1);
//	}
//	if (*VULP(base+TXIR) & BIT2) {
//		*VULP(base+TXMCR) &= ~BIT6;
//		*VULP(base+TXIR) = BIT2;
//		int_handler(timer + 2);
//	}
//	if (*VULP(base+TXIR) & BIT3) {
//		*VULP(base+TXMCR) &= ~BIT9;
//		*VULP(base+TXIR) = BIT3;
//		int_handler(timer + 3);
//	}
//
//	VICVectAddr = 0;	// acknowledge interrupt (if using VIC IRQ)
//}

//This procedure is used for 32-Bit permanent timer
static void timer2_init(uint32_t cpsr) {
//	PCONP |= PCTIM0;		// power up timer
//	T0TCR = 2;				// disable and reset timer
//	T0MCR = 0;				// disable compare
//	T0CCR = 0;				// capture is disabled
//	T0EMR = 0;				// no external match output
//	T0PR = cpsr;			// set prescaler
//	install_irq(TIMER0_INT, &timer_irq, 1);
//	T0TCR = 1;				// reset counter
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//TIM_TimeBaseStructure.TIM_Period = 100 - 1; // 1 MHz down to 10 KHz (0.1 ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 840 -1; /*1 msec reso*/ //cpsr; // Down to 1 MHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//not relevant in this case
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Regular);
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
	int_handler = handler;

	SYSTICK_RELOAD = HWTIMER_MAXTICKS;
	SYSTICK_CONTROL = SYSTICK_CONTROL_ENABLE | SYSTICK_CONTROL_TICKINT;
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_enable_interrupt(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	// Timer Interrupt konfigurieren
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_Init(&NVIC_InitStructure);
//	VICIntEnable = 1 << TIMER0_INT;	/* Enable Interrupt */
//	VICIntEnable = 1 << TIMER1_INT;	/* Enable Interrupt */
//	VICIntEnable = 1 << TIMER2_INT;	/* Enable Interrupt */
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_disable_interrupt(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	// Timer Interrupt konfigurieren
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_Init(&NVIC_InitStructure);
//	VICIntEnClr = 1 << TIMER0_INT;	/* Disable Interrupt */
//	VICIntEnClr = 1 << TIMER1_INT;	/* Disable Interrupt */
//	VICIntEnClr = 1 << TIMER2_INT;	/* Disable Interrupt */
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
	switch (timer) {
		case 0: {
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
			break;
		}
		case 1: {
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
			break;
		}
		case 2: {
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
			break;
		}
		case 3: {
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, DISABLE);
			break;
		}
	}
/*	volatile unsigned long base = get_base_address(timer);
	timer %= 4;
	*VULP(base+TXMCR) &= ~(MR0I << (3 * timer));	// disable interrupt for match register
	 *VULP(base+TXIR) = 0x01 << timer;				// reset interrupt register value for corresponding match register
	 *VULP(base+TXIR) */
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
