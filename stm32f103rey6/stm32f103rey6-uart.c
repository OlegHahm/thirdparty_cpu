/*
 * Copyright (C) 2014 INRIA 
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu 
 * @{
 *
 * @file        stm32f103rey6-uart.c
 * @brief       STM32F103 USART IRQ handler 
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "stm32f10x.h"
#include "cpu.h"
#include "sched.h"
#include "board_uart0.h"

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
__attribute__ ((naked)) void USART1_IRQHandler(void)
{
    interrupt_entry();
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */

        int c = USART_ReceiveData(USART1);
#ifdef MODULE_UART0
        if (uart0_handler_pid) {
            uart0_handle_incoming(c);

            uart0_notify_thread();
        }
#else
        (void)c;
#endif
    }
    if (sched_context_switch_request) {
        thread_yield();
    }

    interrupt_return();
}
