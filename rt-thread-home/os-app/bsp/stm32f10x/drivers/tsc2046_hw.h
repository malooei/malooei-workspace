/*
 * Description:	tsc2046 hardware init 
 */

#define TSC2046_HW_H_

#include <stm32f10x.h>

#define BIT(n)       			(1<<n)  //set_bit
#define SET(reg, n)   			reg |= BIT(n)
#define CLR(reg, n)    			reg &= ~(BIT(n))

/*
 * 2014/6/25 了解tsc2046触摸屏控制芯片
 */  
#define TSC2046_HW_DEBUG   1  
#define TSC2046_12BITMODE  1   
#define TSC2046_DEVICE_ID  
  
/*
 * SPI2
 * PB12 -- NSS, PB13 -- SCK, PB14 -- MISO, PB15 -- MOSI
 */
#define SPI2_RCC_PERIPH			RCC_APB1Periph_SPI2
#define SPI2_PORT 			GPIOB
#define SPI2_NSS_PORT 			GPIOB
#define PENIRQ_PORT			GPIOG	
#define SPI2_NSS			GPIO_Pin_12
#define SPI2_SCK			GPIO_Pin_13
#define SPI2_MISO			GPIO_Pin_14
#define SPI2_MOSI			GPIO_Pin_15
#define PENIRQ_PIN			GPIO_Pin_7


/* spi mode */
#define TSC2046_SPIX			SPI2
#define TSC2046_SPI_RCC_PERIPH		SPI2_RCC_PERIPH
#define TSC2046_SPI_PORT 		SPI2_PORT
#define TSC2046_SPI_NSS_PORT		SPI2_NSS_PORT
#define TSC2046_SPI_SCK			SPI2_SCK
#define TSC2046_SPI_MISO		SPI2_MISO
#define TSC2046_SPI_MOSI		SPI2_MOSI 
#define TSC2046_SPI_NSS			SPI2_NSS
#define TSC2046_SPI_DUMMY 		0XFF

/* control gpio */
/* PENIRQ */
#define TSC2046_PENIRQ_PIN		PENIRQ_PIN
#define TSC2046_PENIRQ_PORT		PENIRQ_PORT
#define TSC2046_PENIRQ_PORT_SOURCE	GPIO_PortSourceGPIOG
#define TSC2046_PENIRQ_PIN_SOURCE	GPIO_PinSource7
#define TSC2046_PENIRQ_EXT_LINE		EXTI_Line7
#define TSC2046_PENIRQ_INT_NUM 		EXTI9_5_IRQn

#define TSC2046_PREEMPTION_PRI		0
#define TSC2046_SUB_PRI			0


#define turnhigh_tsc2046_cs()     	GPIO_SetBits(TSC2046_SPI_PORT, TSC2046_SPI_NSS)
#define turnlow_tsc2046_cs()      	GPIO_ResetBits(TSC2046_SPI_PORT, TSC2046_SPI_NSS)
 

/* tsc2046 control byte */
/*
 *  tsc2046读数据
 *  按时序读，先写控制字，然后再读
 *  tsc2046 control byte
 *  BIT7	BIT6	BIT5	BIT4	BIT3	BIT2	BIT1	BIT0
 *  (MSB)							(LSB)
 *  S		A2      A1      A0      MODE	SER/DFR	PD1	PD0	
 *  BIT7->S->Initiate START
 *  BIT6,5,4->Addressing->Channel Select bits->001-x  101-y
 *  BIT3->MODE->12bit-1  8bit-0
 *  BIT2->SER/DFR->single-end-1 
 */
#define TSC2046_XPOSITION		0	  
#define TSC2046_YPOSITION		1
    
#if TSC2046_12BITMODE      
#define XP_CONTROLBYTE			0x90	/* x-12bit-singlend-PENIRQ_OFF */
#define YP_CONTROLBYTE			0xd0	/* y-12bit-singlend-PENIRQ_OFF */
//#define XP_CONTROLBYTE			0x94	/* x-12bit-singlend-PENIRQ_ON */
//#define YP_CONTROLBYTE			0xd4	/* y-12bit-singlend-PENIRQ_ON */
#else
#define XP_CONTROLBYTE			0x98	/* x-8bit-singlend-PENIRQ_OFF */
#define YP_CONTROLBYTE			0xd8	/* y-8bit-singlend-PENIRQ_OFF */
//#define XP_CONTROLBYTE			0x9C	/* x-8bit-singlend-PENIRQ_ON */
//#define YP_CONTROLBYTE			0xdC	/* y-8bit-singlend-PENIRQ_ON */

#endif


extern void tsc2046_delay_penirq(vu32 ncount);
extern void start_tsc2046(void);
extern unsigned int read_tsc2046_valx(u32 cmd);

    


