#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <stm32_inside_function_api.h>
#include <stm32_inside_function_hw.h>



static ADC_InitTypeDef 	ADC_InitStructure;


#if STM32_INSIDE_RTC
static void inside_rtc_rcc_init(void)
{

	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	/* Enable BKP */
	PWR_BackupAccessCmd(ENABLE);

	BKP_DeInit();
	/* RTC USE LSE Clock */
	RCC_LSEConfig(RCC_LSE_ON);
    
	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){
		rt_kprintf("wait inside rtc ready....\n");
		?/* 注；智能控制箱硬件上没有LSE32.768khz晶振 */
	} 
    
	
 	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);

	RTC_WaitForSynchro();
	RTC_WaitForLastTask();

	/* Enable RTC sec it */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();

	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	RTC_WaitForLastTask();	

}


void start_inside_rtc(void)
{
#if STM32_INSIDE_RTC_DEBUG
	rt_kprintf("start stm32 inside RTC...\n");
#endif
	inside_rtc_rcc_init();
}



#endif



#if STM32_INSIDE_TEMP
static void adc_inside_temp_configuration(void)
{   
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	ADC_DeInit(ADC1); 
 	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		
 	ADC_InitStructure.ADC_ScanConvMode = DISABLE;		
 	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		
 	ADC_InitStructure.ADC_NbrOfChannel = 1;
 	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		
 	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						
	ADC_Init(ADC1, &ADC_InitStructure);
	  
        ADC_RegularChannelConfig(ADC1, ADC_Channel_16,  1, ADC_SampleTime_239Cycles5);  //恒温盒内温度

	ADC_TempSensorVrefintCmd(ENABLE);  //测温度的 使能
    
 	//ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

 	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
 	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


static void inside_temp_hw_init(void)
{
	adc_inside_temp_configuration();
}



void start_inside_temp(void)
{ 
#if STM32_INSIDE_TEMP_DEBUG
	rt_kprintf("start stm32 inside TEMP...\n");
#endif
	inside_temp_hw_init();
}

#endif






