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

#ifndef CPUCONF_H_
#define CPUCONF_H_

/**
 * @ingroup		conf
 * @ingroup		stm32f103rey6
 *
 * @{
 */

/**
 * @file
 * @brief		STM32f103 CPUconfiguration
 *
 * @author    Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		baar
 * @author    Alaeddine Weslati <alaeddine.weslati@intia.fr>
 * @version
 *
 *
 */
#define TRANSCEIVER_BUFFER_SIZE (3)

#define FEUERWARE_CONF_CPU_NAME			"stm32f103rey6"

/**
 * @name CPU capabilities
 * @{
 */
//#define FEUERWARE_CPU_LPC2387					1
#define FEUERWARE_CONF_CORE_SUPPORTS_TIME		1
/** @} */

/**
 * @name Stdlib configuration
 * @{
 */
#define __FOPEN_MAX__		4
#define __FILENAME_MAX__	12
/** @} */

/**
 * @name Kernel configuration
 * @{
 */
#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT	512
#endif

#define KERNEL_CONF_STACKSIZE_IDLE		512

#define KERNEL_CONF_STACKSIZE_PRINTF_FLOAT  (4096)
#define KERNEL_CONF_STACKSIZE_PRINTF        (2048)

#define UART0_BUFSIZE                   (128)

/** @} */

/**
 * @name Compiler specifics
 * @{
 */
#define CC_CONF_INLINE					inline
#define CC_CONF_USED					__attribute__((used))
#define CC_CONF_NONNULL(...)			__attribute__((nonnull(__VA_ARGS__)))
#define CC_CONF_WARN_UNUSED_RESULT		__attribute__((warn_unused_result))
/** @} */

#define CPU_ID_ADDR     (0x1ffff7e8)
#define CPU_ID_LEN      (12)

/** @} */
#endif /* CPUCONF_H_ */
