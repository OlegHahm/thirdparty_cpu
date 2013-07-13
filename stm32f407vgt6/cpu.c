/*
 * cpu.c
 *
 *  Created on: 07.09.2012
 *      Author: pfeiffer
 */

#include <stdint.h>

#include "stm32f4xx.h"

#include "cpu.h"
#include "sched.h"

extern void sched_task_exit(void);

static inline void save_context(void);
static inline void restore_context(void);

__attribute__((always_inline))
static inline void save_context(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */

    asm("ldr    r1, =active_thread");
    /* load address of currend pdc */
    asm("ldr    r1, [r1]");
    /* deref pdc */
    asm("cmp    r1, #0");
    /* active_thread exists */
    asm("ittt   ne");
    /*if active_thread != NULL */
    asm("pushne {r4-r11}");
    /* save unsaved registers */
    /*vstmdb    sp!, {s16-s31}    */ //FIXME save fpu registers if needed
    asm("pushne {LR}");
    /* save exception return value */
    asm("strne  sp, [r1]");
    /* write sp to pdc->sp means current threads stack pointer */
}

__attribute__((always_inline))
static inline void restore_context(void)
{
    asm("ldr    r0, =active_thread");
    /* load address of currend pdc */
    asm("ldr    r0, [r0]");
    /* deref pdc */
    asm("ldr    sp, [r0]");
    /* load pdc->sp to sp register */

    asm("pop    {r0}");
    /* restore exception retrun value from stack */
    /*pop       {s16-s31}        */ //FIXME load fpu register if needed depends on r0 exret
    asm("pop    {r4-r11}");
    /* load unloaded register */
    //    asm("pop         {r4}"); /*foo*/
    asm("bx     r0");                /* load exception return value to pc causes end of exception*/
    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

void cpu_switch_context_exit(void)
{
    __pendSV();
    __enable_irq();
}

void thread_yield(void)
{
    __pendSV();
}

__attribute__((naked))
void PendSV_Handler(void)
{
    save_context();
    sched_run();
    restore_context();
}

/*
 * cortex m4 knows stacks and handles register backups
 *
 * so use different stack frame layout
 *
 *
 * with float storage
 * ------------------------------------------------------------------------------------------------------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR | S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 | S10 | S11 | S12 | S13 | S14 | S15 | FPSCR |
 * ------------------------------------------------------------------------------------------------------------------------------------
 *
 * without
 *
 * --------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR |
 * --------------------------------------
 *
 *
 */
char *thread_stack_init(void *task_func, void *stack_start, uint32_t stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)(stack_start + stack_size);

    /* marker */
    stk--;
    *stk = 0x77777777;

    //FIXME FPSCR
    stk--;
    *stk = (uint32_t) 0;

    //S0 - S15
    for (int i = 15; i >= 0; i--) {
        stk--;
        *stk = i;
    }

    //FIXME xPSR
    stk--;
    *stk = (uint32_t) 0x01000200;

    //program counter
    stk--;
    *stk = (uint32_t) task_func;

    /* link register */
    stk--;
    *stk = (uint32_t) sched_task_exit;

    /* r12 */
    stk--;
    *stk = (uint32_t) 0;

    /* r0 - r3 */
    for (int i = 3; i >= 0; i--) {
        stk--;
        *stk = i;
    }

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* foo */
    /*stk--;
    *stk = (uint32_t) 0xDEADBEEF;*/

    /* lr means exception return code  */
    stk--;
    *stk = (uint32_t) 0xfffffff9; // return to taskmode main stack pointer

    return (char *) stk;
}
