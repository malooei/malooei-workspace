#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <w25x16_hw.h>
#include <w25x16_api.h>

/*
2012/9/18 DMA方式实现SPI Flash 在指定地址指定字节数的串口读取（加地址偏移，若要返回值需重新偏移接收数组地址！！切记）
2012/9/18 正在编写DMA方式写入数据函数   

2012/9/18 DMA方式实现SPI Flash读取的方案二，不用DMA方式发送命令与地址，直接发送四个字节！
          只接受就行！    
2012/9/19 DMA方式实现SPI Flash读取和写入（不带自动擦除写页）的方案二实现！
          但是这种连续启动若干次DMA的传输方式貌似有问题？不问题，怀疑是与连续启动打乱了时序有关
*/

#define W25X16_HW_DEBUG   0
#define W25X16_DEVICE_ID  0xEF14


/*
 * W25X SPI Flash芯片的硬件端口配置
 */
static void w25x_gpio_init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOC , ENABLE); 
	
	/* w25x spi接口数据端 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	/* w25x 片选信号端 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);  

	turnhigh_w25x_cs();
}


/*
 * W25X SPI Flash芯片的spi通信端口配置
 */
static void w25x_spi_init(void)
{

	SPI_InitTypeDef SPI_InitStructure;

	SPI_Cmd(W25X_SPIX, DISABLE);  
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;     
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;        
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;   
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(W25X_SPIX,&SPI_InitStructure);  

	SPI_Cmd(W25X_SPIX,ENABLE); 

}


static void w25x_hw_init(void)
{
	w25x_gpio_init(); 
	w25x_spi_init();
}


static void w25x_judge_id(void)
{
	u32 read_id;

	read_id = w25x_read_id();

	if(W25X16_DEVICE_ID == read_id){
		rt_kprintf("init w25x16 hardware success...\n");		
	}else{
		rt_kprintf("init w25x16 hardware fail...\n");
	}  

#if W25X16_HW_DEBUG
	rt_kprintf("read w25x id:0x%x\n", read_id);
#endif

}  


/*
 * W25X SPI Flash初始化配置
 */
void start_w25x16(void)
{
	w25x_hw_init();
	w25x_judge_id();
}
 

unsigned int w25x_readwrite_byte(u8 data) 
{
	/* 如果发送寄存器数据没有发送完，循环等待  */      
	while(SPI_I2S_GetFlagStatus(W25X_SPIX,SPI_I2S_FLAG_TXE)==RESET);
	     SPI_I2S_SendData(W25X_SPIX,data);
	/*  如果接收寄存器没有收到数据，循环 */ 
	while(SPI_I2S_GetFlagStatus(W25X_SPIX,SPI_I2S_FLAG_RXNE)==RESET);

	return SPI_I2S_ReceiveData(W25X_SPIX);
}


unsigned int w25x_flash_readbyte(void)
{
	return (w25x_readwrite_byte(0xff));
}


#if 0
void SPI1_DMA1_Read_Init_Configuration(u32 Addr,u16 NumByteToRead)
{
	u16 j=0;                          /*测试*/		   
	u8 SPI1_DMA1_Rx_BUF[4096];
	u8 SPI1_DMA1_ReadComAddr_Table[4];
	SPI1_DMA1_ReadComAddr_Table[0]=0x03;
	   	 /*命令*/  /*24位地址*/
	SPI1_DMA1_ReadComAddr_Table[1]=(u8)((Addr)>>16);
	SPI1_DMA1_ReadComAddr_Table[2]=(u8)((Addr)>>8);
	SPI1_DMA1_ReadComAddr_Table[3]=(u8)(Addr);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 

	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);

	   /*DMA1通道3配置SPI1_TX,此配置只用于发送要读取的命令和起始地址*/
	DMA_DeInit(DMA1_Channel3);
	DMA_StructInit(&DMA_InitStructure);

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;		 //外设的基地址
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)SPI1_DMA1_ReadComAddr_Table; 	  //存储器基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;		 //规定外设作为目的地还是来源
	DMA_InitStructure.DMA_BufferSize=NumByteToRead+4; 	     //数据单位，有待理解 ?
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//用于设置外设地址寄存器递增与否,外设地址不增，只为传输命令和地址
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;		  //用于设置内存地址寄存器递增与否
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte; //与MemoryDataSize有什么区别？
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;  //设置外设数据宽度为8位
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;  //设置优先级
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;     		  //没有设置内存到内存的传输
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);	

	/*	  方案二：待测，先搞定方案一
	turnlow_w25x_cs();
	w25x_readwrite_byte(READ_DATA);
	w25x_readwrite_byte(0x00);
	w25x_readwrite_byte(0x00);
	w25x_readwrite_byte(0x00);	 
	turnhigh_w25x_cs();     
	*/

	/*DMA1通道2配置SPI1_RX,此配置用于接收SPI Flash发送来的数据，数据存入SPI1_DMA1_Rx_BUF*/
	DMA_DeInit(DMA1_Channel2);
	DMA_StructInit(&DMA_InitStructure);
	//DMA_InitStructure.DMA_PeripheralBaseAddr=0x000000;                   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_DMA1_Rx_BUF;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize =NumByteToRead+4;	  // ?
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //此用于设置外设地址是否加一		
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);	


	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);  


	turnlow_w25x_cs();
	DMA_Cmd(DMA1_Channel2, ENABLE);

	DMA_Cmd(DMA1_Channel3, ENABLE);


	while( !DMA_GetFlagStatus(DMA1_FLAG_TC2));
	while( !DMA_GetFlagStatus(DMA1_FLAG_TC3));

	for(j=0;j<NumByteToRead;j++){
		USART1_Write_ByteToPc(SPI1_DMA1_Rx_BUF[j+4]);
	}
} 

void SPI1_DMA1_Write_Init_Configuration(u32 Addr,u8*Buffer,u16 NumByteToWrite)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 

	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);

	/* DMA1通道3配置SPI1_TX,此配置只用于发送要读取的命令和起始地址 */

	DMA_DeInit(DMA1_Channel3);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)Buffer; 
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;	
	DMA_InitStructure.DMA_BufferSize=NumByteToWrite;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;		
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte; 
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte; 
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_High; 
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;     	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);	

	/* 方案二：待测，先搞定方案一
	turnlow_w25x_cs();
	w25x_readwrite_byte(READ_DATA);
	w25x_readwrite_byte((u8)((Addr)>>16));
	w25x_readwrite_byte((u8)((Addr)>>8));
	w25x_readwrite_byte((u8)(Addr));	 
	turnhigh_w25x_cs();     
	*/

	/* DMA1通道2配置SPI1_RX,此配置用于接收SPI Flash发送来的数据，数据存入SPI1_DMA1_Rx_BUF */
	/*
	DMA_DeInit(DMA1_Channel2);
	DMA_StructInit(&DMA_InitStructure);
	//DMA_InitStructure.DMA_PeripheralBaseAddr=0x000000;                   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_DMA1_Tx_BUF;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize =NumByteToWrite+4;	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);	
	*/   

	/*

	注意：在使用中断之前一定要使能相应中断，例如应用串口接收，那么在使用该中断前
	就要先使能中断！

	*/

	//DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);	

	//DMA_ITConfig(DMA1_Channel2,DMA_IT_TC, ENABLE);

	/*发送缓冲DMA使能，当此为被设置时，TXE标志被设置时，产生一个DMA请求*/

	//SPI_I2S_DMACmd(SPI1, SPI1_DMA1_Tx_BUF, ENABLE);

	//SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);  

	w25x_write_enable();
	turnlow_w25x_cs();
	w25x_readwrite_byte(0x02);
	w25x_readwrite_byte((u8)((Addr)>>16));
	w25x_readwrite_byte((u8)((Addr)>>8));
	w25x_readwrite_byte((u8)(Addr));


	//DMA_Cmd(DMA1_Channel2, ENABLE);

	DMA_Cmd(DMA1_Channel3, ENABLE);

	USART1_Write_ByteToPc(0x00);		  ///////致命问题，为什么要加这个，不然写入数据有问题

	while( !DMA_GetFlagStatus(DMA1_FLAG_TC3));

	turnhigh_w25x_cs();

	USART1_Write_ByteToPc(0x01);		  ///////致命问题，为什么要加这个，不然写入数据有问题


	/*

	* @arg DMA1_FLAG_GL3: DMA1 Channel3 global flag.
	* @arg DMA1_FLAG_TC3: DMA1 Channel3 transfer complete flag.
	* @arg DMA1_FLAG_HT3: DMA1 Channel3 half transfer flag.
	* @arg DMA1_FLAG_TE3: DMA1 Channel3 transfer error flag.
	DMA1_Channel3状态标志位！

	*/


	/*

	没有数据输出，说明死在该循环内，传输根本没有进行，那就没有不可能置位
	所以目前问题在DMA启动上

	*/ 

	//while( !DMA_GetFlagStatus(DMA1_FLAG_TC2));
	//while( !DMA_GetFlagStatus(DMA1_FLAG_TC3));	 /*解决问题的突破口v001_m001*/ 

}

int main(void)
{
    RCC_Configuration();   //配置系统时钟，设置系统时钟为72M 
    NVIC_Configuration();  //配置中断
 
    
    USART1_GPIO_Configuration();
    USART1_Init_Configuration();

	w25x_spi_init();
   
  	w25x_erase_sector(0);
    /*DMA方式读取片外SPIFlash  SPI1_DMA1_Read_Init_Configuration(u32 Addr,u16 NumByteToRead) */

            // SPI1_DMA1_Read_Init_Configuration(10,100);


 	/*测试不带自动擦除DMA方式的写页*/
  
	SPI1_DMA1_Write_Init_Configuration(0,WriteToW25X16_1,20);
	SPI1_DMA1_Write_Init_Configuration(20,WriteToW25X16_2,20);
	SPI1_DMA1_Write_Init_Configuration(40,WriteToW25X16_1,20);
	SPI1_DMA1_Write_Init_Configuration(60,WriteToW25X16_2,20);
	SPI1_DMA1_Write_Init_Configuration(80,WriteToW25X16_1,20);
	SPI1_DMA1_Write_Init_Configuration(100,WriteToW25X16_2,20);
   
		   	//w25x_Write(0,WriteToW25X16_1,20);
		    //w25x_Write(4096,WriteToW25X16_2,20);

           //USART1_W25X_DMABUF_Display(40);

        //SPI1_DMA1_Read_Init_Configuration(0,50);
   	USART1_W25X_Display_Read(0,140);  /*从地址为0x00000000的地址开始读第一页*/


}
#endif

#if W25X16_HW_DEBUG
void cmd_debug_hw(int cmd, unsigned data)
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
FINSH_FUNCTION_EXPORT(cmd_debug_hw, cmd_debug);
#endif


