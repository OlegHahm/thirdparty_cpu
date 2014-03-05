#include "cpu.h"
#include "sched.h"
#include "hwtimer.h"

#include "board.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void (*int_handler)(int);

unsigned long hwtimer_arch_now(void)
{
    return TIM_GetCounter(TIM2);
}

/*---------------------------------------------------------------------------*/

__attribute__((naked))
void TIM2_IRQHandler(void)
{
    interrupt_entry();
    if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
    {
        DEBUG("Firing 0 at %lu\n", hwtimer_now());
        int_handler(0);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
        TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);
    }
    if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
    {
        DEBUG("Firing 1 at %lu\n", hwtimer_now());
        int_handler(1);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
        TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);
    }
    if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
    {
        DEBUG("Firing 2 at %lu\n", hwtimer_now());
        int_handler(2);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
        TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);
    }
    if(TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
    {
        DEBUG("Firing 3 at %lu\n", hwtimer_now());
        int_handler(3);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
        TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
    interrupt_return();
}

__attribute__((naked))
void TIM3_IRQHandler(void)
{
    save_context();

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        if (!(TIM3->CR1 & TIM_CR1_CEN)) {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
            int_handler(1);
        }

        if (sched_context_switch_request) {
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
    TIM_TimeBaseInitTypeDef tim_init;
    NVIC_InitTypeDef nvic_init;

    // init generic timer options
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;

    // setup the interrupt controller
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    nvic_init.NVIC_IRQChannelSubPriority = 0;

    // enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // timer init
    tim_init.TIM_Period = 0xFFFF;
    tim_init.TIM_Prescaler = 36000;
    TIM_TimeBaseInit(TIM2, &tim_init);
    // irq setup
    nvic_init.NVIC_IRQChannel = TIM2_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Init(&nvic_init);
    // enable timer
    TIM_Cmd(TIM2, ENABLE);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_enable_interrupt(void)
{
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_disable_interrupt(void)
{
    NVIC_DisableIRQ(TIM2_IRQn);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    DEBUG("set absolute timer %hu to %lu\n", timer, value);
    switch (timer) {
        case 0:
            TIM_SetCompare1(TIM2, value);
            TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
            break;
        case 1:
            TIM_SetCompare2(TIM2, value);
            TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
            break;
        case 2:
            TIM_SetCompare3(TIM2, value);
            TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
            break;
        case 3:
            TIM_SetCompare4(TIM2, value);
            TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);
            break;
    }
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set(unsigned long offset, short timer)
{
    unsigned long now = hwtimer_now();
    DEBUG("set timer %hu to %lu + %lu\n", timer, now, offset);
    switch (timer) {
        case 0:
            TIM_SetCompare1(TIM2, now + offset- 1);
            TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
            break;
        case 1:
            TIM_SetCompare2(TIM2, now + offset- 1);
            TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
            break;
        case 2:
            TIM_SetCompare3(TIM2, now + offset- 1);
            TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
            break;
        case 3:
            TIM_SetCompare4(TIM2, now + offset- 1);
            TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);
            break;
    }
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_unset(short timer)
{
    switch (timer) {
        case 0:
            TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);
            break;
        case 1:
            TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);
            break;
        case 2:
            TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);
            break;
        case 3:
            TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);
            break;
    }
}
/*---------------------------------------------------------------------------*/
