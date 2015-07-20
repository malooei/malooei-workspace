#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <enc28j60_hw.h>
#include <enc28j60_api.h>


#define ENC28J60_HW_DEBUG   1

static void enc28j60_delay(vu32 ncount)    //延时函数
{
  
  for(; ncount > 0; ncount--);

}

 
/*
 * ENC28J60 芯片的硬件端口配置
 */
static void enc28j60_gpio_init(void) 
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
    
	/* ENC28J60 spi接口数据端 */
	GPIO_InitStructure.GPIO_Pin   = ENC28J60_SPI_SCK | ENC28J60_SPI_MISO | ENC28J60_SPI_MOSI; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(ENC28J60_SPI_PORT, &GPIO_InitStructure);  

	/* ENC28J60 片选信号端 */
	GPIO_InitStructure.GPIO_Pin   = ENC28J60_SPI_NSS; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(ENC28J60_SPI_PORT, &GPIO_InitStructure);     

	/* ENC28J60 INT */
	GPIO_InitStructure.GPIO_Pin   = ENC28J60_INT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(ENC28J60_INT_PORT, &GPIO_InitStructure);
	
	ENC28J60_CS_HIGH;
}



/*
 * ENC28J60 芯片的spi通信端口配置
 */
static void enc28j60_spi_init(void)
{
	SPI_InitTypeDef SPI_InitStructure;  

	SPI_Cmd(ENC28J60_SPIX, DISABLE);  
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;     
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;        
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;   
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(ENC28J60_SPIX,&SPI_InitStructure);  

	SPI_Cmd(ENC28J60_SPIX, ENABLE);   

}


static void enc28j60_hw_init(void)
{
	enc28j60_gpio_init(); 
	enc28j60_spi_init();
}


/*
 * ENC28J60 芯片外部中断配置 
 */  
static void enc28j60_exti_penirq_config(void)
{

	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(ENC28J60_INT_PORT_SOURCE, ENC28J60_INT_PIN_SOURCE);

	EXTI_InitStructure.EXTI_Line 	= ENC28J60_INT_EXT_LINE;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
	EXTI_Init(&EXTI_InitStructure);

}
  

/*
 * ENC28J60 芯片PENIRQ外部中断优先级配置 
 */
static void enc28j60_nvic_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = ENC28J60_INT_NUM;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ENC28J60_PREEMPTION_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ENC28J60_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
 

/*
 * enc28j60初始化配置
 */
void start_enc28j60(void)
{ 
#if ENC28J60_HW_DEBUG
	rt_kprintf("start enc28j60...\n");
#endif

	enc28j60_hw_init();
	enc28j60_exti_penirq_config(); 
	enc28j60_nvic_config();
}
  

unsigned int enc28j60_readwrite_byte(u8 data) 
{

	/* 如果发送寄存器数据没有发送完，循环等待  */      
	while(SPI_I2S_GetFlagStatus(ENC28J60_SPIX,SPI_I2S_FLAG_TXE)==RESET);
	     SPI_I2S_SendData(ENC28J60_SPIX,data);
	/*  如果接收寄存器没有收到数据，循环 */ 
	while(SPI_I2S_GetFlagStatus(ENC28J60_SPIX,SPI_I2S_FLAG_RXNE)==RESET);

	return SPI_I2S_ReceiveData(ENC28J60_SPIX);
}


unsigned int enc28j60_readbyte(void)
{
	return (enc28j60_readwrite_byte(0xff));
}


/**
 * Receives multiple bytes from the SPI bus and writes them to a buffer.
 *
 * \param[out] buffer A pointer to the buffer into which the data gets written.
 * \param[in] buffer_len The number of bytes to read.
 */
void spix_rec_data(SPI_TypeDef* SPIx, u8* buffer, u16 buffer_len, unsigned dummy)
{
	while (buffer_len--) {
#if 0		
		while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_TXE));
		spi_i2s_send_byte(SPIx, dummy); 

		while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_RXNE));
		*buffer++ = spi_i2s_recv_byte(SPIx);
#else
		*buffer++ = enc28j60_readbyte();
#endif  
	}

	return;
}


/**
 * Sends data contained in a buffer over the SPI bus.
 *
 * \param[in] data A pointer to the buffer which contains the data to send.
 * \param[in] data_len The number of bytes to send.
 */
void spix_send_data(SPI_TypeDef* SPIx, const u8* data, u16 data_len)
{
	uint8_t b;
	while (data_len--) {
		b = *data++;

#if 0
		while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_TXE));
		spi_i2s_send_byte(SPIx, b);
		//Wait until a data is received
		while(!is_spix_sr_flag_set(SPIx, SPI_I2S_FLAG_RXNE));
		spi_i2s_recv_byte(SPIx);
#else
		enc28j60_readwrite_byte(b);
  
#endif
	}

	return;
}



