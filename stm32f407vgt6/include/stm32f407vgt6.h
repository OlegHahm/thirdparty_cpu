/*
 * stm32f407vgt6.h
 *
 *  Created on: 08.08.2012
 *      Author: pfeiffer
 */

#ifndef STM32F407VGT6_H_
#define STM32F407VGT6_H_

#include "stm32f4xx.h"

//#define F_CCO					288000000
//#define CL_CPU_DIV				4									///< CPU clock divider
#define F_CPU					168000000						///< CPU target speed in Hz
//#define F_RC_OSCILLATOR			4000000								///< Frequency of internal RC oscillator
//#define F_RTC_OSCILLATOR		32767								///< Frequency of RTC oscillator
//#define PCLK_DIV                0x2                                 ///< PCLK clock divider, F_CPU/PCLK_DIV=PCLK

#define VIC_SIZE                32

#define GPIO_INT 17
#define IRQP_GPIO 4



#endif /* STM32F407VGT6_H_ */
