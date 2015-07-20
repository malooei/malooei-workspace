#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <fsmc_sram_hw.h>
#include <fsmc_sram_api.h>

#if FSMC_SRAM_HW_DEBUG
void cmd_sram_hw(int cmd, unsigned data)
{  
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
FINSH_FUNCTION_EXPORT(cmd_sram_hw, fsmc_sram_debug);
#endif



