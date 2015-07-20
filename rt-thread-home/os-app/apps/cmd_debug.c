#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <w25x16_api.h>
#include <stm32_inside_function_api.h>
#include <enc28j60_api.h>

u8 test_send_data[128]="keep moving ! the power of dream!!!";
u8 read_buffer[128];


void cmd_debug(int cmd, int data, int data1)
{
	s32 temp;
	u8 cnt;

	
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
#if STM32_INSIDE_TEMP
		/* test stm32 inside temp */
		temp = get_inside_temp();
		rt_kprintf("read stm32 inside average TEMP: %d\n", temp);
#endif
		break;  

	case 5:    
#if STM32_INSIDE_RTC
		/* test stm32 inside rtc */
		read_inside_rtc();
#endif			
		break;

	case 6:
#if STM32_EXT_AM2301
		/* test stm32 ext am2301 */	
		read_ext_am2301();
#endif
		break;

	case 7:  
#if STM32_EXT_ENC28J60
		rt_kprintf("test debug enc28j60...\n");

		for(cnt = 0; cnt < data1; cnt++){

			rt_kprintf("-----------------test clear and set bit: %d-----------------\n", cnt);

			temp = enc28j60_read(EBSTCON); 

			rt_kprintf("read MIWRL reg 1st: %d\n", temp);
			enc28j60_write(EBSTCON,data);  

			temp = enc28j60_read(EBSTCON);
			rt_kprintf("read MIWRL reg 2st: %d\n", temp);

			enc28j60_set_bits(EBSTCON, cnt);
			temp = enc28j60_read(EBSTCON);		
			rt_kprintf("set_bits: %d\nread MIWRL reg 3st: %d\n", data1, temp);
			
			enc28j60_clear_bits(EBSTCON, cnt);
			temp = enc28j60_read(EBSTCON);		
			rt_kprintf("clear_bits: %d\nread MIWRL reg 4st: %d\n", data1, temp);
		}
#endif 
		break;
 
	case 8:
		enc28j60_soft_reset(); 

		break; 
	
	case 9:
		//enc28j60_write_buffer(test_send_data, data);
 
		break;

	case 10:
		//enc28j60_read_buffer(read_buffer,data);

		break;

	case 11:

		rt_kprintf("-----------------test phy reg read and write-----------------\n"
			   "----------------------- LED1ÁÁ LED2Ãð -----------------------\n");

		temp = enc28j60_read_phy(PHLCON); 
		rt_kprintf("read PHLCON reg 1st: %d\n", temp);

		enc28j60_write_phy(PHLCON, 0x89e); /* LED1ÁÁ LED2Ãð */  
 
		temp = enc28j60_read_phy(PHLCON);
		rt_kprintf("read PHLCON reg 2st: %d\n", temp);

		break;  

	case 12:   

		rt_kprintf("-----------------test phy reg read and write-----------------\n"
			   "----------------------- LED1Ãð LED2ÁÁ -----------------------\n");
 
		temp = enc28j60_read_phy(PHLCON); 
		rt_kprintf("read PHLCON reg 1st: %d\n", temp);

		enc28j60_write_phy(PHLCON, 0x98e); /* LED1Ãð LED2ÁÁ */	
 
		temp = enc28j60_read_phy(PHLCON);
		rt_kprintf("read PHLCON reg 2st: %d\n", temp);

		break;

	case 13: 
		rt_kprintf("test phy PHID1 reg\n");	
		
		temp = enc28j60_read_phy(PHID1); 
		rt_kprintf("read PHID1 reg 1st: %d\n", temp);
  

		break;

	case 14: 

		break;		
		
	case 15: 
		rt_kprintf("send test buffer:\n%s\n",test_send_data);	
		
		break;		

	case 16: 
		rt_kprintf("send test buffer:\n%s\n",test_send_data);	

		break;		


	
	default:
		break;
	}
	
	return;
}
FINSH_FUNCTION_EXPORT(cmd_debug, cmd_debug);


