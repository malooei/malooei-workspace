#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <tsc2046_hw.h>
#include <tsc2046_api.h>
#include "ssd1289.h"

u32 adjust_touch_screen_flag = 0;
u32 turn_point_screen_flag = 0;

void touch_screen_init(void)
{
	
 
}


void set_adejust_touch_st(int num)
{
/*
	struct calctouch *touch_st;
	em_info->chlx_st[0].pa_vgain
*/
}


/*
 *
 * �������ѹ����:
 * ����
 * ���� cmd_ssd1289_api(13,0,10,10,6)
 * ���� cmd_ssd1289_api(13,0,10,302,6)
 * ���� cmd_ssd1289_api(13,0,225,302,6)
 * ���� cmd_ssd1289_api(13,0,225,10,6)
 *
 */  
void adjust_touch_screen(int cmd)
{
	adjust_touch_screen_flag = 1;
	turn_point_screen_flag = 1;
	
	switch(cmd){
	case 0:
		/* У׼���ϵ� */
	    lcd_clear(White);  
		ssd1289_lcd_set_pixel_size(0,10,10,6);	
		do{

			rt_kprintf("test adjust touch...\n");

		}while(adjust_touch_screen_flag);
  
		break;
	
	case 1:
		/* У׼���ϵ� */
	    lcd_clear(White);  
		ssd1289_lcd_set_pixel_size(0,10,302,6);

		break;

	case 2:
		/* У׼���µ� */
	    lcd_clear(White);  
		ssd1289_lcd_set_pixel_size(0,225,302,6);

		break;

	case 3:  
		/* У׼���µ� */
	    lcd_clear(White);  
		ssd1289_lcd_set_pixel_size(0,225,10,6);

		break;

	default:
		break;
	}

	return;

}  


#if TSC2046_API_DEBUG    
void cmd_tsc2046_api(int cmd, int data, int data1)
{    
	switch(cmd){
	case 0:
     	rt_hw_led_on(2);
		break;

	case 1:
       	//rt_hw_led_off(2);
		break;

	case 2:
       	rt_hw_led_on(3);
		break;

	case 3:  
       	rt_hw_led_off(3);
		break;
  
	case 4:       
		adjust_touch_screen(data);
		break;

	case 5:    
		break;

	case 6:
		break;

	default:
		break;
	}
	
	return;
}
FINSH_FUNCTION_EXPORT(cmd_tsc2046_api, cmd_debug);
#endif  

  
