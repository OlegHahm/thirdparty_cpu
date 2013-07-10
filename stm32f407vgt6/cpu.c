/*
 * cpu.c
 *
 *  Created on: 07.09.2012
 *      Author: pfeiffer
 */

#include "stdint.h"
#include "stm32f4xx.h"

#include "cpu.h"
#include "sched.h"

extern void sched_task_exit(void);
void sched_task_return(void);

void stm32f4_pclk_scale(uint32_t source, uint32_t target, uint32_t* pclksel, uint32_t* prescale)
{
	uint32_t pclkdiv;
	*prescale = source / target;

	if( (*prescale % 4) == 0 ) {
		*pclksel = TIM_CKD_DIV4;
		pclkdiv = 4;
	} else if( (*prescale % 2) == 0 ) {
		*pclksel = TIM_CKD_DIV2;
		pclkdiv = 2;
	} else {
		*pclksel = TIM_CKD_DIV1;
		pclkdiv = 1;
	}
	*prescale /= pclkdiv;

	if( *prescale % 2 )
		(*prescale)++;
}

void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t* prescale) {
    uint32_t pclksel;

    stm32f4_pclk_scale(source, target, &pclksel, prescale);

    //PCLKSEL0 = (PCLKSEL0 & ~(BIT2|BIT3)) | (pclksel << 2); 		// timer 0
    //PCLKSEL0 = (PCLKSEL0 & ~(BIT4|BIT5)) | (pclksel << 4); 		// timer 1
    //PCLKSEL1 = (PCLKSEL1 & ~(BIT12|BIT13)) | (pclksel << 12);	// timer 2
}

void cpu_switch_context_exit(void){
    sched_run();
    sched_task_return();
}

void thread_yield(void) {
	asm("svc 0x01\n");
}

__attribute__((naked))
void SVC_Handler(void)
{
	save_context();
	asm("bl sched_run");
	/* call scheduler update active_thread variable with pdc of next thread
	 * the thread that has higest priority and is in PENDING state */
	restore_context();
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

	/* save user mode stack pointer in *active_thread */
	asm("ldr     r1, =active_thread"); /* r1 = &active_thread */
	asm("ldr     r1, [r1]"); /* r1 = *r1 = active_thread */
	asm("str     r12, [r1]"); /* store stack pointer in tasks pdc*/

	sched_task_return();
}

/* call scheduler so active_thread points to the next task */
void sched_task_return(void)
{
	/* load pdc->stackpointer in r0 */
	asm("ldr     r0, =active_thread"); /* r0 = &active_thread */
	asm("ldr     r0, [r0]"); /* r0 = *r0 = active_thread */
	asm("ldr     sp, [r0]"); /* sp = r0  restore stack pointer*/
	asm("pop		{r4}"); /* skip exception return */
	asm(" pop		{r4-r11}");
	asm(" pop		{r0-r3,r12,lr}"); /* simulate register restor from stack */
//	asm("pop 		{r4}"); /*foo*/
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
char * thread_stack_init(void * task_func, void * stack_start, int stack_size ) {
	unsigned int * stk;
	stk = (unsigned int *) (stack_start + stack_size);

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
	*stk = (unsigned int) 0xfffffff9; // return to taskmode main stack pointer

	return (char*) stk;
}
