/*
 * Description:
 * stm32 inside function api.h
 */
 
#define STM32_INSIDE_FUNCTION_API_H_

#include "stm32f10x.h"

#define STM32_INSIDE_FUN_API_DEBUG		 0  
/* RTC in stm32 */
#define STM32_INSIDE_RTC_DEBUG			 1
/* temperature in stm32 */
#define STM32_INSIDE_TEMP_DEBUG			 1

extern s32 get_inside_temp(void);
extern void read_inside_rtc(void);




