/*
 * Description:
 * w25x16 api.h
 *
 */
#define W25X16_API_H_

#include "stm32f10x.h"

#define W25X16_API_DEBUG 0  
/*
 * description: w25x16 status register
 
#define W25X_SR_BUSY     0
#define W25X_SR_WEL      1
#define W25X_SR_BP0      2
#define W25X_SR_BP1      3
#define W25X_SR_BP2      4
#define W25X_SR_TB       5
#define W25X_SR_R        6
#define W25X_SR_SRP      7
*/
#if W25X16_API_DEBUG
#define W25X_ADDR_BEGIN    0x000000 /* the begining address of external SRAM */
#define W25X_ADDR_END      0x000000 /* the end address of external SRAM */
#endif  

enum w25x16_status_reg {
	W25X_SR_BUSY		= 0,
	W25X_SR_WEL,
	W25X_SR_BP0,	
	W25X_SR_BP1,		
	W25X_SR_BP2,
	W25X_SR_TB,	
	W25X_SR_R,
	W25X_SR_SRP,	
};

#define BIT_R_SHIFT(data, n)  data>>(n)
#define BIT_SET		1
#define BIT_CLR		0

extern unsigned int analyse_w25x_sr(int id);
extern unsigned int w25x_read_sr(void); 
extern unsigned int w25x_read_id(void);
extern void w25x_write_sr(u8 data); 
extern void w25x_write_enable(void); 
extern void w25x_erase_sector(u32 addr);  
extern void w25x_write_disable(void); 
extern void w25x_read(u32 addr, u8 *bytebutter, u32 bytebuttersize);  
extern void w25x_write_page(u32 addr,u8* butter,u16 numbytetoread);
extern void w25x_write_nocheck(u32 addr, u8* buffer, u16 numbytetowrite);
extern void w25x_Write(u32 addr,u8* buffer,u16 numbytetowrite);
extern void w25x_erase_chip(void);
extern void w25x_waitbusy(void);
extern void w25x_powerdown(void);
extern void w25x_wakeup(void);

