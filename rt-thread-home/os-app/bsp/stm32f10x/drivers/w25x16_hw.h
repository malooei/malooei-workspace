/*
 * Description:	w25x16 hardware init 
 */

#define W25X16_HW_H_

#include <stm32f10x.h>

#define READ_SR		 0x05
#define WRITE_SR	 0x01
#define WRITE_DISABLE	 0x04 
#define READ_ID		 0x90
#define READ_DATA 	 0x03
#define WRITE_PAGE 	 0x02
#define ERASE_CHIP 	 0xc7
#define ERASE_SECTOR 	 0x20
#define POWER_DOWN  	 0xb9
#define WAKE_UP 	 0xab
#define WRITE_ENABLE 	 0x06

#define W25X_SPIX	 SPI1
#define W25X_CS_PORT 	 GPIOC
#define W25X_CS_PIN  	 GPIO_Pin_4

#define turnhigh_w25x_cs()      GPIO_SetBits(W25X_CS_PORT, W25X_CS_PIN)
#define turnlow_w25x_cs()       GPIO_ResetBits(W25X_CS_PORT, W25X_CS_PIN)
/*
extern void w25x_gpio_init(void);
extern void w25x_spi_init(void);
*/
extern void start_w25x16(void);
extern unsigned int w25x_readwrite_byte(u8 data); 
extern unsigned int w25x_flash_readbyte(void);

