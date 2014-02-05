#include <cpu.h>
#include <sched.h>
#include <debug.h>

#include "board.h"

#define DEBUG

static void (*int_handler)(int);

// TODO
unsigned long hwtimer_arch_now(void) {
  return 0;
}

/*---------------------------------------------------------------------------*/

__attribute__((naked))
void TIM2_IRQHandler(void) {
  save_context();
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    if(!(TIM2->CR1 & TIM_CR1_CEN)){
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
      int_handler(0);
    }
    if(sched_context_switch_request)
    {
      // scheduler
      sched_run();
    }
  }
  restore_context();
}

__attribute__((naked))
void TIM3_IRQHandler(void) {
  save_context();
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(!(TIM3->CR1 & TIM_CR1_CEN)){
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
      int_handler(1);
    }
    if(sched_context_switch_request)
    {
      // scheduler
      sched_run();
    }
  }
  restore_context();
}

__attribute__((naked))
void TIM4_IRQHandler(void) {
  save_context();
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    if(!(TIM4->CR1 & TIM_CR1_CEN)){
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
      int_handler(2);
    }
    if(sched_context_switch_request)
    {
      // scheduler
      sched_run();
    }
  }
  restore_context();
}

__attribute__((naked))
void TIM5_IRQHandler(void) {
  save_context();
  if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    if(!(TIM5->CR1 & TIM_CR1_CEN)){
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);
      int_handler(3);
    }
    if(sched_context_switch_request)
    {
      // scheduler
      sched_run();
    }
  }
  restore_context();
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
  DEBUG("hwtimer : arch_init\n");
  int_handler = handler;
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_enable_interrupt(void) {
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_disable_interrupt(void) {
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set_absolute(unsigned long value, short timer) {
  uint32_t TIMx;

  uint16_t prescaler;
  uint16_t period;

  if (value >= (1<<16)) {
    prescaler = 36000;    // 2 KhZ
    period = value / 500;
  } else {
    prescaler = 72;       // 1 MHz
    period = value;
  }

  DEBUG("hwtimer : setting timer %u to : %u, prescaler %u, period %u \n", 
          timer, value, prescaler, period);

  switch(timer) {
    case 0: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); break;
    case 1: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); break;
    case 2: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); break;
    case 3: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); break;
  }

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = period;

  switch(timer) {
    case 0: TIMx = TIM2; break;
    case 1: TIMx = TIM3; break;
    case 2: TIMx = TIM4; break;
    case 3: TIMx = TIM5; break;
  }

  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

  TIM_SelectOnePulseMode(TIMx, TIM_OPMode_Single);

  TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
  TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

  /* enable counter */
  TIM_Cmd(TIMx, ENABLE);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set(unsigned long offset, short timer) {
  hwtimer_arch_set_absolute(offset, timer);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_unset(short timer) {
  switch (timer) {
    case 0: {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
      break;
    }
    case 1: {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
      break;
    }
    case 2: {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
      break;
    }
    case 3: {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);
      break;
    }
  }
}

/*---------------------------------------------------------------------------*/
