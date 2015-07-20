#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <am2301_hw.h>
#include <am2301_api.h>



  
    
/*
 * TSC2046 оƬ��Ӳ���˿�����
 */
static void am2301_gpio_init(void) 
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(AM2301_DATA_RCC | RCC_APB2Periph_AFIO, ENABLE);   
              
	/* am2301 �ӿ����ݶ� */            
	GPIO_InitStructure.GPIO_Pin   = AM2301_DATA_PIN | GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      
	GPIO_Init(AM2301_DATA_PORT, &GPIO_InitStructure);    
  
	turnhigh_am2301_data();
}
  

static void am2301_hw_init()
{
	am2301_gpio_init();
}

  
 /*
  * am2301��ʼ������
  */ 
 void start_ext_am2301(void)
{
#if AM2301_HW_DEBUG
	rt_kprintf("start am2301...\n");
#endif

	am2301_hw_init();
}




