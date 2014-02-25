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

#include "board_uart0.h"

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
    thread_print_all();
    puts("HARD FAULT");
    reboot();

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

NORETURN void reboot(void)
{
    while (1) {
        NVIC_SystemReset();
    }
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    interrupt_entry();
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */

#ifdef MODULE_UART0
        if (uart0_handler_pid) {
            int c = USART_ReceiveData(USART1);
            uart0_handle_incoming(c);

            uart0_notify_thread();
        }
#endif
        /* Disable the USART1 Receive interrupt */
        //USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    }

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        /* Write one byte to the transmit data register */

        /* Disable the USART1 Transmit interrupt */
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
    interrupt_return();
}
