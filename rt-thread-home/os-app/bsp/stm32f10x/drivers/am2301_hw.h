/*
 * Description:	am2301 hardware init 
 * am2301 is a temperature and humidity sensor
 *
 */

#define AM2301_HW_H_

#include <stm32f10x.h>


#define AM2301_HW_DEBUG 	1  


/* am2301 通信接口属于单总线串行通信 通信只需一个IO口控制 */
#define DATA_PORT			GPIOE
#define DATA_PIN			GPIO_Pin_6
#define DATA_RCC			RCC_APB2Periph_GPIOE
  
  
#define AM2301_DATA_PIN			DATA_PIN
#define AM2301_DATA_PORT		DATA_PORT
#define AM2301_DATA_RCC			DATA_RCC

#define turnhigh_am2301_data()     	GPIO_SetBits(AM2301_DATA_PORT, AM2301_DATA_PIN)
#define turnlow_am2301_data()      	GPIO_ResetBits(AM2301_DATA_PORT, AM2301_DATA_PIN)

#define waitlow_data_passby()		do{} while(!GPIO_ReadInputDataBit(AM2301_DATA_PORT, AM2301_DATA_PIN))	
#define waithigh_data_passby()		do{} while(GPIO_ReadInputDataBit(AM2301_DATA_PORT, AM2301_DATA_PIN))	

  
/*
 *
 * am2301 sample data:temperature and humidity
 * 数据格式: 40bit数据=16bit湿度数据+16bit温度数据+8bit校验和  
 *
 */

struct am2301_sample_st{
	u32 temp;
	u32 humidity;
	u32 check;
};





