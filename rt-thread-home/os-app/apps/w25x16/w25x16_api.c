#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <w25x16_hw.h>
#include <w25x16_api.h>



#define CNT_TIM 	 500

/*
 * description: analyse w25x16 status register
 *
 * status register 8bit:
 *
 * bit 0: BUSY  ERASE OR WRITE IN PROGRESS 
 * bit 1: WEL   WRITE ENABLE LATCH
 * bit 2: BP0   BLOCK PROTECT BITS
 * bit 3: BP1   BLOCK PROTECT BITS
 * bit 4: BP2   BLOCK PROTECT BITS
 * bit 5: TB    TOP/BOTTOM WRITE PROTECT
 * bit 6: (R)   RESERVED
 * bit 7: SRP   STATUS REGISTER PROTECT
 *
 */
unsigned int analyse_w25x_sr(int id)
{
	u32 sr_val;

	sr_val = w25x_read_sr();  
	if(BIT_R_SHIFT(sr_val, id)&0x1 == BIT_SET){
		return 1;
	}else{
		return 0;
	}
}


static void delay(u32 ncount)
{
	for(;ncount>0;ncount--);
}


/*
 * w25x 读状态寄存器  test pass...
 */
unsigned int w25x_read_sr(void) 
{
	u8 data;
	turnlow_w25x_cs();
	w25x_readwrite_byte(READ_SR);
	data = w25x_readwrite_byte(0xff);
	turnhigh_w25x_cs();
	return data;
}


/*
 * w25x 写状态寄存器
 */
void w25x_write_sr(u8 data) 
{
	turnlow_w25x_cs();
	w25x_readwrite_byte(WRITE_SR);
	w25x_readwrite_byte(data);
	turnhigh_w25x_cs();
}


/*
 * w25x 写使能  test pass..
 */
void w25x_write_enable(void) 
{
	turnlow_w25x_cs();
	w25x_readwrite_byte(WRITE_ENABLE);
	turnhigh_w25x_cs();
}


/*
 * 等待空闲
 */
void w25x_waitbusy(void)  
{

#if 1
	u32 cnt;
	
	cnt = 100000;  
	while(BIT_SET == analyse_w25x_sr(W25X_SR_BUSY)){
		delay(CNT_TIM);	  
		cnt--;  
		if(cnt == 0){ 
			rt_kprintf("w25x waitbusy too long time...\n");	
			break;
		}
	}
#else
	while((w25x_read_sr()&0x01)==0x01);  
#endif
}


/*
 * 扇区擦除 
 */
void w25x_erase_sector(u32 addr)  
{
	addr *= 4096;
	w25x_write_enable();
	w25x_waitbusy();
	turnlow_w25x_cs();
	w25x_readwrite_byte(ERASE_SECTOR);
	w25x_readwrite_byte((u8)((addr) >> 16));
	w25x_readwrite_byte((u8)((addr) >> 8));
	w25x_readwrite_byte((u8)(addr));
	turnhigh_w25x_cs();

	w25x_write_disable();	
	w25x_waitbusy();
}


/*
 * 写禁能  test pass..
 */
void w25x_write_disable(void) 
{
	turnlow_w25x_cs();
	w25x_readwrite_byte(WRITE_DISABLE);
	turnhigh_w25x_cs();
}


/*
 * 获取器件ID, 16bit数据 test pass...
 */
unsigned int w25x_read_id(void)
{
	u32 value = 0;

	turnlow_w25x_cs();
	
	w25x_readwrite_byte(READ_ID);
	w25x_readwrite_byte(0x00);
	w25x_readwrite_byte(0x00);
	w25x_readwrite_byte(0x00);
	value |= w25x_readwrite_byte(0xFF) << 8;
	value |= w25x_readwrite_byte(0xFF);

	turnhigh_w25x_cs();

	return value;
}


/*
 * description: resd data w25x16 chip
 * 
 * addr: data store in w25x start address
 * bytebutter: data remap to master chip start pointer
 * bytebuttersize: data in master chip remap size
 *	
 */
void w25x_read(u32 addr, u8 *bytebutter, u32 bytebuttersize)  
{
	u16 i;  
	
	turnlow_w25x_cs();
	w25x_readwrite_byte(READ_DATA);
	w25x_readwrite_byte((u8)((addr) >> 16));
	w25x_readwrite_byte((u8)((addr) >> 8));
	w25x_readwrite_byte((u8)(addr));
#if 0
	for(i = 0; i < numbytetoread; i++ ){	
		butter[i] = w25x_readwrite_byte(0xFF);
	}
#else
	while(bytebuttersize--)
		*bytebutter++ = w25x_readwrite_byte(0xFF);	
#endif
	turnhigh_w25x_cs(); 
}


void w25x_write_page(u32 addr,u8* butter,u16 numbytetoread)  ///写页，写不多于一页的数据
{
	u32 i;
	w25x_write_enable();
	turnlow_w25x_cs();

	w25x_readwrite_byte(WRITE_PAGE);
	w25x_readwrite_byte((u8)((addr) >> 16));
	w25x_readwrite_byte((u8)((addr) >> 8));
	w25x_readwrite_byte((u8)(addr));

	for(i = 0; i < numbytetoread; i++){
		w25x_readwrite_byte(butter[i]);   	  
	}

	turnhigh_w25x_cs(); 
	w25x_write_disable();
	w25x_waitbusy(); //等待写入完成，要保证时间，当写入完成后BUSY位会置0；
}


/*
 * 说明：无校验写SPIF,前提需保证所写地址的内容为0xff，否则会出错误.
 *	 调用函数w25x_write_page()写页！
 *	 在spi flash操作，写入数据时，只能从1到0，不能从0变为1，这也是写入数据前需保证数据为0xff原因!
 * 注意：此函数的意思重要，可作为SPI Flash读写的标准写 MARK!
 *	 在按页写时，当写满地址会自动跳变到页的首地址，所以在按扇区进行写时
 *	 是以按页写为单元，即需要写入调用页写入函数，所以应及时更新变量，每写新页就要更新一次！
 */
void w25x_write_nocheck(u32 addr, u8* buffer, u16 numbytetowrite) 
{
	u16 pageremain; //单页剩余字节
	pageremain = 256 - addr % 256;

	if(numbytetowrite <= pageremain){
		/* 判断是否满足单页写入的条件 */
		pageremain = numbytetowrite;
	}

	while(1){
		/* 需更新每页首地址对于的数据指针，并更新写入页的首地址和剩余写入字节数 */
		/* 注意：此写入调用页写入函数，所以应及时更新变量，每写新页就要更新一次！*/
		w25x_write_page(addr, buffer, pageremain);
		if(numbytetowrite == pageremain){
			break;
		}else{
			/* 处理跨页剩余数据,地址关系 */
			buffer += pageremain;
			addr += pageremain;	
			numbytetowrite -= pageremain;

			if(numbytetowrite > 256){
				pageremain = 256;
			}else{
				numbytetowrite = pageremain;
			}
		} 
	}
}


/*
 * description: 按照指定地址，指定长度（字节数）写SPIF
 *		sectorpos 	扇区块，编号0~255，在变换扇区时 加1 起连续写扇区作用！ 
 *		sectorcos 	扇区的偏移地址，也很重要，起定位判断操作！
 *		sectorremain    扇区内剩余能写入的字节数
 *		sector_buf	读出一个扇区的数据，用作数据拼接作用
 */
void w25x_write(u32 addr, u8* buffer, u16 numbytetowrite)
{
	u32 sectorpos;	 
	u16 sectorcos;	 
	u16 sectorremain; 
	u16 i = 0;
	static u8 sector_buf[4096];
	
	sectorpos = addr / 4096;
	sectorcos = addr % 4096;
	sectorremain = 4096 - sectorcos;

	if(numbytetowrite <= sectorremain){
		sectorremain = numbytetowrite;  
	}

	while(1){
		w25x_read(sectorpos * 4096, sector_buf, 4096); 
		for(i = 0; i < sectorremain; i++){
			/* 有数据则跳出,需要拼接数据 */
			if(sector_buf[i+sectorcos] != 0xff)
				break;				
		}

		if(i < sectorremain){
			w25x_erase_sector(sectorpos);

			/* 所写扇区不纯净纯净 */  
			/* 在缓存中拼接,并按扇区写入拼接后的数据 */
			for(i = 0; i < sectorremain; i++){
				sector_buf[i+sectorcos] = buffer[i];
			}

			w25x_write_nocheck(sectorpos * 4096, sector_buf, 4096);
		}else{    
			w25x_write_nocheck(addr, buffer, sectorremain);/* 所写扇区纯净 */  
		}
		  
		//判断数据是否结束写入
		if(sectorremain == numbytetowrite){
			break;
		}else{
			//数据量大于一个扇区，需要继续写入，知道不满一扇区为止！
			sectorpos++;
			sectorcos = 0;
			buffer += sectorremain;
			addr += sectorremain;
			numbytetowrite -= sectorremain;
  
			if(numbytetowrite > 4096){
				sectorremain = 4096;
			}else{
				sectorremain = numbytetowrite;
			}
		}

	}
}


/*
 * 擦除整片
 */
void w25x_erase_chip()
{
	w25x_write_enable();
	w25x_waitbusy();
	turnlow_w25x_cs();
	w25x_readwrite_byte(ERASE_CHIP);
	turnhigh_w25x_cs();
 	w25x_write_disable();
	w25x_waitbusy();
#if W25X16_API_DEBUG  
	rt_kprintf("erase w25x complete...\n");		
#endif
}


/*
 * 进入掉电模式
 */
void w25x_powerdown() 
{
	turnlow_w25x_cs();
	w25x_readwrite_byte(POWER_DOWN);
	turnhigh_w25x_cs();
	delay(100);
}


/*
 * 唤醒(释放掉电)
 */
void w25x_wakeup()	
{
	turnlow_w25x_cs();
	w25x_readwrite_byte(WAKE_UP);
	turnhigh_w25x_cs();
	delay(100);
}

#if W25X16_API_DEBUG
static void test_printf_sr_bit(u32 data)
{
	
	data = analyse_w25x_sr(0);
	rt_kprintf("status register bit%d:0x%x\n", 0, data);

	data = analyse_w25x_sr(1);
	rt_kprintf("status register bit%d:0x%x\n", 1, data);

	data = analyse_w25x_sr(2);
	rt_kprintf("status register bit%d:0x%x\n", 2, data);

	data = analyse_w25x_sr(3);
	rt_kprintf("status register bit%d:0x%x\n", 3, data);

	data = analyse_w25x_sr(4);
	rt_kprintf("status register bit%d:0x%x\n", 4, data);

	data = analyse_w25x_sr(5);
	rt_kprintf("status register bit%d:0x%x\n", 5, data);

	data = analyse_w25x_sr(6);
	rt_kprintf("status register bit%d:0x%x\n", 6, data);

	data = analyse_w25x_sr(7);
	rt_kprintf("status register bit%d:0x%x\n", 7, data);

}

static void test_printf_read_data(u8 *butter, int lenght)
{
	rt_kprintf("read w25x date butter:\n");

	while(lenght--)
		rt_kprintf("0x%08x\n", *butter++);		

}  

    
void cmd_debug_api(int cmd, int data, int data1)
{
	u32 test_data;
	u8 test_butter[1024];
	u8 test_butter1[128] = {"malooei test write w25x16 spi flash ....\n"};
	u8 test_butter2[128] = {"flash spi w25x16 write test malooei ....\n"};
	switch (cmd) {
		case 0:
		     	rt_hw_led_on(2);
			break;

		case 1:
	        	rt_hw_led_off(2);
			break;

		case 2:
	        	rt_hw_led_on(3);
			break;

		case 3:  
	        	rt_hw_led_off(3);
			break;

		case 4: 
			w25x_read_id();
			break;
	  
		case 5:  
			test_data = w25x_read_sr();
			rt_kprintf("w25x16 status register value:0x%x\n", test_data);
			break;

		case 6:
			test_data = w25x_read_sr();
			rt_kprintf("w25x16 status register value:0x%x\n", test_data);
			test_printf_sr_bit(test_data);

			break;

		case 7:
			test_data = w25x_read_sr();
			rt_kprintf("w25x16 status register value:0x%x\n", test_data);
  			test_printf_sr_bit(test_data);

			w25x_write_enable(); 
			test_data = w25x_read_sr();
			rt_kprintf("w25x16 status register value:0x%x\n", test_data);
  			test_printf_sr_bit(test_data);

			w25x_write_disable(); 
			test_data = w25x_read_sr();
			rt_kprintf("w25x16 status register value:0x%x\n", test_data);
  			test_printf_sr_bit(test_data);
			  
			break;

		case 8:  
			w25x_write_disable(); 
			rt_kprintf("w25x16 status register value:0x%x\n", test_data);
  			test_printf_sr_bit(test_data);
			
			break;
			
		case 9:
			w25x_read(data, &test_butter[0], data1);
			test_printf_read_data(&test_butter[0], data1);

			break;  

		case 10:  
			w25x_erase_chip();
			break;

		case 11:  
			//w25x_write_page(data, &test_butter1[0], sizeof(test_butter1));
			w25x_write_nocheck(data, &test_butter1[0], sizeof(test_butter1));
			break;

		case 12:     
			w25x_read(data, &test_butter[0], sizeof(test_butter));

			rt_kprintf("read data from w25x :\n");		
			rt_kprintf("%s ",test_butter);						
			rt_kprintf("\n");						

			break;  
		case 13:    
			//w25x_write_page(data, &test_butter2[0], sizeof(test_butter2));
			w25x_write_nocheck(data, &test_butter2[0], sizeof(test_butter2));
			break;  
		case 14:
			w25x_write(data, &test_butter2[0], sizeof(test_butter2));
			
			break;
		default:
			break;
	}
	
	return;
}
FINSH_FUNCTION_EXPORT(cmd_debug_api, cmd_debug);
#endif  
  
