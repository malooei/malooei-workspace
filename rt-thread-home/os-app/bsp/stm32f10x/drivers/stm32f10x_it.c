/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <board.h>
#include <rtthread.h>
#include <tsc2046_hw.h>
  


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
#if TSC2046_HW_DEBUG
s32 test_tsc2046_flag;
#endif
extern u32 adjust_touch_screen_flag;
extern u32 turn_point_screen_flag;

void EXTI9_5_IRQHandler(void)
{     
	u8  flag;  
	u32 datax = 0, datay = 0;
	EXTI->PR = EXTI_Line7;

	//rt_kprintf("\n**enter ture touch screen exti9_5IRQHandler\n");
	/* enter interrupt */ 
	rt_interrupt_enter();     
      
#if TSC2046_HW_DEBUG  
	tsc2046_delay_penirq(200000); /* 200000»Ìº˛—” ±40ms */ 
  
    if(0 == GPIO_ReadInputDataBit(TSC2046_PENIRQ_PORT, TSC2046_PENIRQ_PIN)){

	//rt_kprintf("\nenter ture touch screen exti9_5IRQHandler... cnt:%d\n", test_tsc2046_flag);

	if(test_tsc2046_flag%2 == 0){ 
	     	rt_hw_led_on(2);
		//rt_kprintf("led turn off\n");		     	
	}else{  
	     	rt_hw_led_off(2); 
		//rt_kprintf("led turn on\n");		     	
	}    
	          
	datax = read_tsc2046_valx(TSC2046_XPOSITION);
	datay = read_tsc2046_valx(TSC2046_YPOSITION);
	//rt_kprintf("\nmeasure touch screen x-position :%d y-position :%d\n" ,datax ,datay);
	rt_kprintf("\nx :%d y :%d\n" ,datax ,datay);

    }  
		
	test_tsc2046_flag++;  

	adjust_touch_screen_flag = 0;
	turn_point_screen_flag = 0;
#endif	

	/* leave interrupt */
	rt_interrupt_leave();

	return;
}


/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

//void SysTick_Handler(void)
//{
//    // definition in boarc.c
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

#ifdef  RT_USING_LWIP
/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
    extern void rt_dm9000_isr(void);

    /* enter interrupt */
    rt_interrupt_enter();

    /* Clear the DM9000A EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line4);

    rt_dm9000_isr();

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_LWIP */

/**
  * @}
  */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
