#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <tsc2046_hw.h>
#include <tsc2046_api.h>


void tsc2046_delay_penirq(vu32 ncount)    //延时函数
{
  
  for(; ncount > 0; ncount--);

}


/*
 * TSC2046 芯片的硬件端口配置
 */
static void tsc2046_gpio_init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOG, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
    
	/* tsc2046 spi接口数据端 */
	GPIO_InitStructure.GPIO_Pin   = TSC2046_SPI_SCK | TSC2046_SPI_MISO | TSC2046_SPI_MOSI; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(TSC2046_SPI_PORT, &GPIO_InitStructure);  

	/* tsc2046 片选信号端 */
	GPIO_InitStructure.GPIO_Pin   = TSC2046_SPI_NSS; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(TSC2046_SPI_PORT, &GPIO_InitStructure);     

	/* tsc2046 PENIRQ */
	GPIO_InitStructure.GPIO_Pin   = TSC2046_PENIRQ_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(TSC2046_PENIRQ_PORT, &GPIO_InitStructure);
	
	turnhigh_tsc2046_cs();
	turnlow_tsc2046_cs(); 
	turnhigh_tsc2046_cs();
}


/*
 * TSC2046 芯片的spi通信端口配置
 */
static void tsc2046_spi_init(void)
{

	SPI_InitTypeDef SPI_InitStructure;

	SPI_Cmd(TSC2046_SPIX, DISABLE);  
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;     
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;        
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;   
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(TSC2046_SPIX,&SPI_InitStructure);  

	SPI_Cmd(TSC2046_SPIX, ENABLE); 

}


/*
 * TSC2046 芯片外部中断配置 
 */  
static void tsc2046_exti_penirq_config(void)
{

	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(TSC2046_PENIRQ_PORT_SOURCE, TSC2046_PENIRQ_PIN_SOURCE);

	EXTI_InitStructure.EXTI_Line 	= TSC2046_PENIRQ_EXT_LINE;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
	EXTI_Init(&EXTI_InitStructure);

}
  

/*
 * TSC2046 芯片PENIRQ外部中断优先级配置 
 */
static void tsc2046_nvic_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TSC2046_PENIRQ_INT_NUM;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TSC2046_PREEMPTION_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TSC2046_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}  


static void tsc2046_hw_init(void)
{
	tsc2046_gpio_init(); 
	tsc2046_spi_init();
}


/*
 * tsc2046初始化配置
 */
void start_tsc2046(void)
{
  
#if TSC2046_HW_DEBUG
	rt_kprintf("start tsc246...\n");
#endif 
	tsc2046_hw_init();

	tsc2046_exti_penirq_config(); 
	tsc2046_nvic_config();
}


#define is_spix_sr_flag_set(spix, flag)	(0 != ((spix)->SR & (flag)))
#define spi_i2s_send_byte(spix, data)	((spix)->DR = (unsigned char)data)
#define spi_i2s_recv_byte(spix)		((spix)->DR)


/**
 * Sends a byte over the SPI bus.
 *
 * \param[in] b The byte to send.
 */
uint8_t spix_send_byte(SPI_TypeDef* SPIx, unsigned char writedat)
{
	//Wait until the transmit buffer is empty
	while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_TXE));
	spi_i2s_send_byte(SPIx, writedat);

	//Wait until a data is received
	while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_RXNE));

	return spi_i2s_recv_byte(SPIx);
}


/**
 * Receives a byte from the SPI bus.
 *
 * \returns The received byte.
 */
uint8_t spix_rec_byte(SPI_TypeDef* SPIx, unsigned dummy)
{
	while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_TXE));
	spi_i2s_send_byte(SPIx, dummy); 

	//Wait until a data is received
	while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_RXNE));

	return spi_i2s_recv_byte(SPIx);
}


unsigned int read_tsc2046_valx(u32 cmd)
{
	u32 ret_value = 0;
	u8  tc1 = 0, tc2 = 0;

	SPI_Cmd(TSC2046_SPIX, ENABLE);
	
	turnhigh_tsc2046_cs();
	turnlow_tsc2046_cs();

	switch(cmd){
	
	case TSC2046_XPOSITION:	
		spix_send_byte(TSC2046_SPIX, XP_CONTROLBYTE);
	
		break;
		
	case TSC2046_YPOSITION:
		spix_send_byte(TSC2046_SPIX, YP_CONTROLBYTE);

		break;
  
	default:
		break;
	}    
           
	tsc2046_delay_penirq(20000); /* 200000软件延时40ns */ 
	//rt_kprintf("\n1st tc1:%d  tc2:%d reture_val:%d\n", tc1, tc2, ret_value); 
    
#if TSC2046_12BITMODE 	
	tc1 = spix_rec_byte(TSC2046_SPIX, TSC2046_SPI_DUMMY);
	tc2 = spix_rec_byte(TSC2046_SPIX, TSC2046_SPI_DUMMY);
	turnhigh_tsc2046_cs(); 
   
	ret_value |= tc1 << 4;    
	ret_value |= tc2 >> 4;    
#else
	tc2 = spix_rec_byte(TSC2046_SPIX, TSC2046_SPI_DUMMY);
	turnhigh_tsc2046_cs();
   
	ret_value |= tc2;    
#endif
	//rt_kprintf("\n2st tc1:%d  tc2:%d reture_val:%d\n", tc1, tc2, ret_value); 

	SPI_Cmd(TSC2046_SPIX, DISABLE);

	return ret_value;
}

  
#if TSC2046_HW_DEBUG   
void cmd_tsc2046_hw(int cmd, unsigned data)
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
FINSH_FUNCTION_EXPORT(cmd_tsc2046_hw, cmd_debug);
#endif



