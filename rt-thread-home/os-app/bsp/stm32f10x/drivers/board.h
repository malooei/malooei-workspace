/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f10x.h"

#if STM32_EXT_ENC28J60
#include <enc28j60_hw.h>
#include <enc28j60_api.h>
#endif

/* board configuration */

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0  
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define STM32_SRAM_SIZE         64
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

// 	<i>Enable External W25X16 SPI Flash
#define STM32_EXT_W25X16           0
  
// 	<i>Enable External TSC2046 TOUCH S
#define STM32_EXT_TSC2046          1
  
// 	<i>Enable Stm32 Inside RTC
#define STM32_INSIDE_RTC	   0  

// 	<i>Enable Stm32 Inside TEMP
#define STM32_INSIDE_TEMP	   0

// 	<i>Enable External AM2301 Temperature and humidity sensor
#define STM32_EXT_AM2301	   0  
 
// 	<i>Enable External Ethernet controller
#define STM32_EXT_ENC28J60	   0  

// 	<i>Enable External TFTLCD controller
#define STM32_EXT_SSD1289	   1   



/* USART driver select. */
#define RT_USING_UART1
#define RT_USING_UART2
#define RT_USING_UART3

#endif /* __BOARD_H__ */

// <<< Use Configuration Wizard in Context Menu >>>
