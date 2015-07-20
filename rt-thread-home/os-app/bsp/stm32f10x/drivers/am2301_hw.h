/*
 * Description:	am2301 hardware init 
 * am2301 is a temperature and humidity sensor
 *
 */

#define AM2301_HW_H_

#include <stm32f10x.h>


#define AM2301_HW_DEBUG 	1  


/* am2301 ͨ�Žӿ����ڵ����ߴ���ͨ�� ͨ��ֻ��һ��IO�ڿ��� */
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
 * ���ݸ�ʽ: 40bit����=16bitʪ������+16bit�¶�����+8bitУ���  
 *
 */

struct am2301_sample_st{
	u32 temp;
	u32 humidity;
	u32 check;
};





