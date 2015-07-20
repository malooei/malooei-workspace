/*
 * Description:
 * fsmc sram api.h
 * sram->IS61LV25616 CHIP
 */
#define FSMC_SRAM_H_

#include "stm32f10x.h"

#define FSMC_SRAM_API_DEBUG 0  

extern void fsmc_sram_write(u16* pbuffer, u32 offsetaddr, u32 halfwordtowrite);
extern void fsmc_sram_read(u16* pbuffer, u32 offsetaddr, u32 halfwordtoread);


