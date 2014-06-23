/*
 * cpu.c
 *
 *  Created on: 07.09.2012
 *      Author: pfeiffer
 */
#include <stdio.h>
#include <stdint.h>
#include "stm32f10x_tim.h"
#include "attributes.h"
#include "kernel.h"
#include "crash.h"

#include "board_uart0.h"

extern void sched_task_exit(void);
void sched_task_return(void);

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
void HardFault_Handler(void)
{
    core_panic(HARD_FAULT, "HARD FAULT");

    while (1);
}

__attribute__((naked))
void BusFault_Handler(void)
{
    puts("BusFault_Handler");

    while (1);
}

__attribute__((naked))
void Usage_Handler(void)
{
    puts("Usage FAULT");

    while (1);
}

__attribute__((naked))
void WWDG_Handler(void)
{
    puts("WWDG FAULT");

    while (1);
}

int reboot(int mode)
{
    (void)mode;
    while (1) {
        NVIC_SystemReset();
    }
}

void cpu_switch_context_exit(void)
{
    sched_run();
    sched_task_return();
}


void thread_yield(void)
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;    // set PendSV Bit
}

__attribute__((naked))void PendSV_Handler(void)
{
    save_context();
    /* call scheduler update fk_thread variable with pdc of next thread  */
    asm("bl sched_run");
    /* the thread that has higest priority and is in PENDING state */
    restore_context();
}

    __attribute__((naked))
void SVC_Handler(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
    //    asm("push   {LR}");
    /* save exception return value */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    //
    //    asm("pop    {r0}"               );      /* restore exception retrun value from stack */
    asm("bx     LR"                 );      /* load exception return value to pc causes end of exception*/
    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

/* kernel functions */
void ctx_switch(void)
{
    /* Save return address on stack */
    /* stmfd   sp!, {lr} */

    /* disable interrupts */
    /* mov     lr, #NOINT|SVCMODE */
    /* msr     CPSR_c, lr */
    /* cpsid 	i */

    /* save other register */
    asm("nop");

    asm("mov r12, sp");
    asm("stmfd r12!, {r4-r11}");

    /* save user mode stack pointer in *sched_active_thread */
    asm("ldr     r1, =sched_active_thread"); /* r1 = &sched_active_thread */
    asm("ldr     r1, [r1]"); /* r1 = *r1 = sched_active_thread */
    asm("str     r12, [r1]"); /* store stack pointer in tasks pdc*/

    sched_task_return();
}
/* call scheduler so active_thread points to the next task */
void sched_task_return(void)
{
    /* switch to user mode use PSP insteat of MSP in ISR Mode*/
    CONTROL_Type mode;
    mode.w = __get_CONTROL();
    mode.b.SPSEL = 1; // select PSP
    mode.b.nPRIV = 0; // privilege
    __set_CONTROL(mode.w);
    /* load pdc->stackpointer in r0 */
    asm("ldr     r0, =sched_active_thread"); /* r0 = &sched_active_thread */
    asm("ldr     r0, [r0]"); /* r0 = *r0 = sched_active_thread */
    asm("ldr     sp, [r0]"); /* sp = r0  restore stack pointer*/
    asm("pop		{r4}"); /* skip exception return */
    asm("pop		{r4-r11}");
    asm("pop		{r0-r3,r12,lr}"); /* simulate register restor from stack */
    asm("pop		{pc}");
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
char *thread_stack_init(void *task_func, void *stack_start, int stack_size)
{
    unsigned int *stk;
    stk = (unsigned int *)(stack_start + stack_size);

    /* marker */
    stk--;
    *stk = 0x77777777;

    //FIXME FPSCR
    stk--;
    *stk = (unsigned int) 0;

    //S0 - S15
    for (int i = 15; i >= 0; i--) {
        stk--;
        *stk = i;
    }

    //FIXME xPSR
    stk--;
    *stk = (unsigned int) 0x01000200;

    //program counter
    stk--;
    *stk = (unsigned int) task_func;

    /* link register */
    stk--;
    *stk = (unsigned int) sched_task_exit;

    /* r12 */
    stk--;
    *stk = (unsigned int) 0;

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
    *stk = (unsigned int) 0xDEADBEEF;*/

    /* lr means exception return code  */
    stk--;
    *stk = (unsigned int) 0xfffffffd; // return to taskmode main stack pointer

    return (char *) stk;
}
