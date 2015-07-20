#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <stm32_inside_function_api.h>
#include <stm32_inside_function_hw.h>

struct rtc_time_st time_member;


/* read inside rtc time value into rtc_time_st */
void read_inside_rtc(void)
{
	u32 time_val, day_val;

	/* 获取秒数   */
	time_val = RTC_GetCounter();

	day_val = time_val/(24*3600);
	
	time_member.year  = day_val/360;
	time_member.month = (day_val - time_member.year * 360) / 30;
	time_member.day   = (day_val - time_member.year * 360) % 30;
	time_member.month += 1;
	time_member.day   += 1;
	time_member.hour  = (time_val - day_val * 24 * 3600) / 3600;
	time_member.min   = (time_val - day_val * 24 * 3600 - time_member.hour * 3600) / 60;
	time_member.sec   = time_val - day_val * 24 * 3600 - time_member.hour * 3600 - time_member.min * 60;
	

	  
#if STM32_INSIDE_FUN_API_DEBUG
	rt_kprintf("Time: %d:%d:%d  %d:%d:%d \n", time_member.year, time_member.month, time_member.day,
						  time_member.hour, time_member.min  , time_member.sec);
#endif

}



/*
 *	stm32片内温度获取，单次，最小刻度0.1度
 */
s32 get_inside_temp(void)
{  
	u8	cnt, id;
	s32 temp = 0, temp_add = 0;

	for(cnt=0; cnt<4; cnt++){

		ADC_SoftwareStartConvCmd(ADC1 , ENABLE);

		do{
				id = ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC);
		}while(!id);
		

		temp_add = ADC_GetConversionValue(ADC1);
		temp = temp + temp_add;

#if STM32_INSIDE_FUN_API_DEBUG
		rt_kprintf("read stm32 inside TEMP: %d cnt: %d\n", temp_add, cnt);
#endif				

		if(cnt == 3){

			temp = temp>>2;

#if STM32_INSIDE_FUN_API_DEBUG
			rt_kprintf("read stm32 inside average TEMP orig: %d\n", temp);
#endif	

			/*最终温度转换公式2013/3/11 malooei*/
			//temp = (1.43 - temp*3.3/4096) * 1000/4.35 + 13;

			/* 反推 temp(in) = 2780  temp(out) = 291(0.1度) */
			temp = (1430000 - temp * 508) / 435 + 250;

			return temp;
		}  

	}  
}




  
