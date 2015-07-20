/*
 * Description:
 * enc28j60 api.h
 *
 */
#define ENC28J60_API_H_

#include "stm32f10x.h"

#include <enc28j60_hw.h>


#define ENC28J60_API_DEBUG 1  

extern u8 enc28j60_read(u8 address);
extern u16 enc28j60_read_phy(u8 address);
extern void enc28j60_write_phy(u8 address, u16 value);
extern void enc28j60_write(u8 address, u8 value);
extern void enc28j60_read_buffer(u8* buffer, u16 buffer_len);
extern void enc28j60_write_buffer(u8* buffer, u16 buffer_len);
   


