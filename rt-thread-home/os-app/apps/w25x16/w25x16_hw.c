#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <w25x16_hw.h>
#include <w25x16_api.h>

/*
2012/9/18 DMA��ʽʵ��SPI Flash ��ָ����ַָ���ֽ����Ĵ��ڶ�ȡ���ӵ�ַƫ�ƣ���Ҫ����ֵ������ƫ�ƽ��������ַ�����мǣ�
2012/9/18 ���ڱ�дDMA��ʽд�����ݺ���   

2012/9/18 DMA��ʽʵ��SPI Flash��ȡ�ķ�����������DMA��ʽ�����������ַ��ֱ�ӷ����ĸ��ֽڣ�
          ֻ���ܾ��У�    
2012/9/19 DMA��ʽʵ��SPI Flash��ȡ��д�루�����Զ�����дҳ���ķ�����ʵ�֣�
          �������������������ɴ�DMA�Ĵ��䷽ʽò�������⣿�����⣬����������������������ʱ���й�
*/

#define W25X16_HW_DEBUG   0
#define W25X16_DEVICE_ID  0xEF14


/*
 * W25X SPI FlashоƬ��Ӳ���˿�����
 */
static void w25x_gpio_init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOC , ENABLE); 
	
	/* w25x spi�ӿ����ݶ� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	/* w25x Ƭѡ�źŶ� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);  

	turnhigh_w25x_cs();
}


/*
 * W25X SPI FlashоƬ��spiͨ�Ŷ˿�����
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
 * W25X SPI Flash��ʼ������
 */
void start_w25x16(void)
{
	w25x_hw_init();
	w25x_judge_id();
}
 

unsigned int w25x_readwrite_byte(u8 data) 
{
	/* ������ͼĴ�������û�з����꣬ѭ���ȴ�  */      
	while(SPI_I2S_GetFlagStatus(W25X_SPIX,SPI_I2S_FLAG_TXE)==RESET);
	     SPI_I2S_SendData(W25X_SPIX,data);
	/*  ������ռĴ���û���յ����ݣ�ѭ�� */ 
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
	u16 j=0;                          /*����*/		   
	u8 SPI1_DMA1_Rx_BUF[4096];
	u8 SPI1_DMA1_ReadComAddr_Table[4];
	SPI1_DMA1_ReadComAddr_Table[0]=0x03;
	   	 /*����*/  /*24λ��ַ*/
	SPI1_DMA1_ReadComAddr_Table[1]=(u8)((Addr)>>16);
	SPI1_DMA1_ReadComAddr_Table[2]=(u8)((Addr)>>8);
	SPI1_DMA1_ReadComAddr_Table[3]=(u8)(Addr);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 

	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);

	   /*DMA1ͨ��3����SPI1_TX,������ֻ���ڷ���Ҫ��ȡ���������ʼ��ַ*/
	DMA_DeInit(DMA1_Channel3);
	DMA_StructInit(&DMA_InitStructure);

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;		 //����Ļ���ַ
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)SPI1_DMA1_ReadComAddr_Table; 	  //�洢������ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;		 //�涨������ΪĿ�ĵػ�����Դ
	DMA_InitStructure.DMA_BufferSize=NumByteToRead+4; 	     //���ݵ�λ���д���� ?
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//�������������ַ�Ĵ����������,�����ַ������ֻΪ��������͵�ַ
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;		  //���������ڴ��ַ�Ĵ����������
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte; //��MemoryDataSize��ʲô����
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;  //�����������ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;  //�������ȼ�
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;     		  //û�������ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);	

	/*	  �����������⣬�ȸ㶨����һ
	turnlow_w25x_cs();
	w25x_readwrite_byte(READ_DATA);
	w25x_readwrite_byte(0x00);
	w25x_readwrite_byte(0x00);
	w25x_readwrite_byte(0x00);	 
	turnhigh_w25x_cs();     
	*/

	/*DMA1ͨ��2����SPI1_RX,���������ڽ���SPI Flash�����������ݣ����ݴ���SPI1_DMA1_Rx_BUF*/
	DMA_DeInit(DMA1_Channel2);
	DMA_StructInit(&DMA_InitStructure);
	//DMA_InitStructure.DMA_PeripheralBaseAddr=0x000000;                   
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_DMA1_Rx_BUF;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize =NumByteToRead+4;	  // ?
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //���������������ַ�Ƿ��һ		
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

	/* DMA1ͨ��3����SPI1_TX,������ֻ���ڷ���Ҫ��ȡ���������ʼ��ַ */

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

	/* �����������⣬�ȸ㶨����һ
	turnlow_w25x_cs();
	w25x_readwrite_byte(READ_DATA);
	w25x_readwrite_byte((u8)((Addr)>>16));
	w25x_readwrite_byte((u8)((Addr)>>8));
	w25x_readwrite_byte((u8)(Addr));	 
	turnhigh_w25x_cs();     
	*/

	/* DMA1ͨ��2����SPI1_RX,���������ڽ���SPI Flash�����������ݣ����ݴ���SPI1_DMA1_Rx_BUF */
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

	ע�⣺��ʹ���ж�֮ǰһ��Ҫʹ����Ӧ�жϣ�����Ӧ�ô��ڽ��գ���ô��ʹ�ø��ж�ǰ
	��Ҫ��ʹ���жϣ�

	*/

	//DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);	

	//DMA_ITConfig(DMA1_Channel2,DMA_IT_TC, ENABLE);

	/*���ͻ���DMAʹ�ܣ�����Ϊ������ʱ��TXE��־������ʱ������һ��DMA����*/

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

	USART1_Write_ByteToPc(0x00);		  ///////�������⣬ΪʲôҪ���������Ȼд������������

	while( !DMA_GetFlagStatus(DMA1_FLAG_TC3));

	turnhigh_w25x_cs();

	USART1_Write_ByteToPc(0x01);		  ///////�������⣬ΪʲôҪ���������Ȼд������������


	/*

	* @arg DMA1_FLAG_GL3: DMA1 Channel3 global flag.
	* @arg DMA1_FLAG_TC3: DMA1 Channel3 transfer complete flag.
	* @arg DMA1_FLAG_HT3: DMA1 Channel3 half transfer flag.
	* @arg DMA1_FLAG_TE3: DMA1 Channel3 transfer error flag.
	DMA1_Channel3״̬��־λ��

	*/


	/*

	û�����������˵�����ڸ�ѭ���ڣ��������û�н��У��Ǿ�û�в�������λ
	����Ŀǰ������DMA������

	*/ 

	//while( !DMA_GetFlagStatus(DMA1_FLAG_TC2));
	//while( !DMA_GetFlagStatus(DMA1_FLAG_TC3));	 /*��������ͻ�ƿ�v001_m001*/ 

}

int main(void)
{
    RCC_Configuration();   //����ϵͳʱ�ӣ�����ϵͳʱ��Ϊ72M 
    NVIC_Configuration();  //�����ж�
 
    
    USART1_GPIO_Configuration();
    USART1_Init_Configuration();

	w25x_spi_init();
   
  	w25x_erase_sector(0);
    /*DMA��ʽ��ȡƬ��SPIFlash  SPI1_DMA1_Read_Init_Configuration(u32 Addr,u16 NumByteToRead) */

            // SPI1_DMA1_Read_Init_Configuration(10,100);


 	/*���Բ����Զ�����DMA��ʽ��дҳ*/
  
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
   	USART1_W25X_Display_Read(0,140);  /*�ӵ�ַΪ0x00000000�ĵ�ַ��ʼ����һҳ*/


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


