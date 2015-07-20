#include <finsh.h>  
#include "ssd1289.h"

// Compatible list:
// ssd1289

//内联函数定义,用以提高性能
#ifdef __CC_ARM                			 /* ARM Compiler 	*/
#define lcd_inline   				static __inline
#elif defined (__ICCARM__)        		/* for IAR Compiler */
#define lcd_inline 					inline
#elif defined (__GNUC__)        		/* GNU GCC Compiler */
#define lcd_inline 					static __inline
#else
#define lcd_inline                  static
#endif

#define rw_data_prepare()               write_cmd(34)


/********* control ***********/
#include "stm32f10x.h"
#include "board.h"

//输出重定向.当不进行重定向时.
#define printf               rt_kprintf //使用rt_kprintf来输出
//#define printf(...)                       //无输出

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE4 is used as ship select signal */
/*
 BANK1 NE1   6000 0000h--63ff ffffh
 BANK1 NE2   6400 0000h--67ff ffffh
 BANK1 NE3   6800 0000h--6bff ffffh
 BANK1 NE4   6c00 0000h--6fff ffffh
*/      
/* RS <==> A0 <==>0x6c000002(NE4 RS=1)*/  
#define LCD_REG              (*((volatile unsigned short *) 0x6c000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x6c000002)) /* RS = 1 */

#define LCD_CONTROL_SSD1289 1                 
             
void LCD_FSMCConfig(void)    
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;  
	FSMC_NORSRAMTimingInitTypeDef  p;
	
    /* FSMC GPIO configure */
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF
                               | RCC_APB2Periph_GPIOG, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

#if 1==LCD_CONTROL_NOTHING
        /*
        FSMC_D0 ~ FSMC_D3
        PD14 FSMC_D0   PD15 FSMC_D1   PD0  FSMC_D2   PD1  FSMC_D3
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /*
        FSMC_D4 ~ FSMC_D12
        PE7 ~ PE15  FSMC_D4 ~ FSMC_D12
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                      | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

        /* FSMC_D13 ~ FSMC_D15   PD8 ~ PD10 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /*
        FSMC_A0 ~ FSMC_A5   FSMC_A6 ~ FSMC_A9
        PF0     ~ PF5       PF12    ~ PF15
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
                                      | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOF,&GPIO_InitStructure);

        /* FSMC_A10 ~ FSMC_A15  PG0 ~ PG5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOG,&GPIO_InitStructure);

        /* FSMC_A16 ~ FSMC_A18  PD11 ~ PD13 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /* RD-PD4 WR-PD5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /* NBL0-PE0 NBL1-PE1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

        /* NE1/NCE2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOD,&GPIO_InitStructure);
        /* NE2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   
        GPIO_Init(GPIOG,&GPIO_InitStructure);
        /* NE3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
        /* NE4 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
		
#elif 1==LCD_CONTROL_SSD1289
	/*
	 FSMC_D0 ~ FSMC_D3
	 PD14 FSMC_D0	PD15 FSMC_D1   PD0	FSMC_D2   PD1  FSMC_D3
	 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
	 GPIO_Init(GPIOD,&GPIO_InitStructure);

	 /*
	 FSMC_D4 ~ FSMC_D12
	 PE7 ~ PE15  FSMC_D4 ~ FSMC_D12
	 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
								   | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	 GPIO_Init(GPIOE,&GPIO_InitStructure);

	 /* FSMC_D13 ~ FSMC_D15   PD8 ~ PD10 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	 GPIO_Init(GPIOD,&GPIO_InitStructure);

	 /*
	 FSMC_A0 ~ RS	 
	 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	 GPIO_Init(GPIOF,&GPIO_InitStructure);

	 /* RD-PD4 WR-PD5 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	 GPIO_Init(GPIOD,&GPIO_InitStructure);

	 /* NBL0-PE0 NBL1-PE1 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	 GPIO_Init(GPIOE,&GPIO_InitStructure);

	 /* NE4 */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	 GPIO_Init(GPIOG,&GPIO_InitStructure);

	 /* BLACK LIGHT  */  
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
     GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;	 
	 GPIO_Init(GPIOA,&GPIO_InitStructure);
	/* turn on black light */
	 GPIO_SetBits(GPIOA, GPIO_Pin_1);
#endif
    }   
    /* FSMC GPIO configure */

    /*-- FSMC Configuration -------------------------------------------------*/
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;	 
	FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);
  
	/* FSMC_Bank1_NORSRAM1 timing configuration */
	p.FSMC_AddressSetupTime = 8;							/*地址建立时间期限*/																						 
	p.FSMC_AddressHoldTime = 8; 							/*地址的持续时间*/
	p.FSMC_DataSetupTime = 8;								/*设定数据时间期限*/
	p.FSMC_BusTurnAroundDuration = 0;						/*总线转向时间*/
	p.FSMC_CLKDivision = 0; 								/*CLK时钟输出信号的HCLK周期数表示时间???*/
	p.FSMC_DataLatency = 0; 								/*指定在获得第一个数据前的时钟周期*/
	p.FSMC_AccessMode = FSMC_AccessMode_A;
   
  
    /* Color LCD configuration ------------------------------------
       LCD configured as follow:
          - Data/Address MUX = Disable
          - Memory Type = SRAM
          - Data Width = 16bit
          - Write Operation = Enable
          - Extended Mode = Enable  
          - Asynchronous Wait = Disable */    

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;  
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
   
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}
  
static void delay(int cnt)
{
    volatile unsigned int dl;
    while(cnt--)
    {
        for(dl=0; dl<500; dl++);
    }
}

static void lcd_port_init(void)
{
    LCD_FSMCConfig();
}

lcd_inline void write_cmd(unsigned short cmd)
{
    LCD_REG = cmd;
}

lcd_inline unsigned short read_data(void)
{
    return LCD_RAM;
}

lcd_inline void write_data(unsigned short data_code )
{
    LCD_RAM = data_code;
}  

lcd_inline void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
    write_cmd(reg_addr);
    write_data(reg_val);
}

lcd_inline unsigned short read_reg(unsigned char reg_addr)
{
    unsigned short val=0;
    write_cmd(reg_addr);
    val = read_data();
    return (val);
}

/********* control <只移植以上函数即可> ***********/

static unsigned short deviceid=0;//设置一个静态变量用来保存LCD的ID

//static unsigned short BGR2RGB(unsigned short c)
//{
//    u16  r, g, b, rgb;
//
//    b = (c>>0)  & 0x1f;
//    g = (c>>5)  & 0x3f;
//    r = (c>>11) & 0x1f;
//
//    rgb =  (b<<11) + (g<<5) + (r<<0);
//
//    return( rgb );
//}

static void lcd_SetCursor(unsigned int x,unsigned int y)
{
    write_reg(0x004e,x);    /* 0-239 */
    write_reg(0x004f,y);    /* 0-319 */
}

/* 读取指定地址的GRAM */
static unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
    unsigned short temp;
    lcd_SetCursor(x,y);
    rw_data_prepare();
    /* dummy read */
    temp = read_data();
    temp = read_data();
    return temp;
}

void lcd_clear(unsigned short Color)
{
    unsigned int index=0;
    lcd_SetCursor(0,0);
    rw_data_prepare();                      /* Prepare to write GRAM */
    for (index=0; index<(LCD_WIDTH*LCD_HEIGHT); index++)
    {
        write_data(Color);
    }
}

static void lcd_data_bus_test(void)
{
    unsigned short temp1;
    unsigned short temp2;
//   /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
//   write_reg(0x0003,(1<<12)|(1<<5)|(1<<4) | (0<<3) );

    /* wirte */
    lcd_SetCursor(0,0);
    rw_data_prepare();
    write_data(0x5555);

    lcd_SetCursor(1,0);
    rw_data_prepare();  
    write_data(0xAAAA);
  
    /* read */
    lcd_SetCursor(0,0);
    temp1 = lcd_read_gram(0,0);
    temp2 = lcd_read_gram(1,0);
  
    if( (temp1 == 0x5555) && (temp2 == 0xAAAA) )
    {
        printf("data bus test pass!\r\n");
    }
    else
    {
        printf("data bus test error: %04X %04X\r\n",temp1,temp2);
    }
}

void ssd1289_init(void)
{
    lcd_port_init();

    deviceid = read_reg(0x00);
  
    /* deviceid check */
    if( deviceid != 0x8989 )
    {
        printf("Invalid LCD ID:%08X\r\n",deviceid);
        printf("Please check you hardware and configure.\r\n");
    }
    else
    {
        printf("\r\nLCD Device ID : %04X \n",deviceid);
    }
  
#if 1
    // power supply setting
    // set R07h at 0021h (GON=1,DTE=0,D[1:0]=01)
    write_reg(0x0007,0x0021);
    // set R00h at 0001h (OSCEN=1)
    write_reg(0x0000,0x0001);
    // set R07h at 0023h (GON=1,DTE=0,D[1:0]=11)
    write_reg(0x0007,0x0023);
    // set R10h at 0000h (Exit sleep mode)
    write_reg(0x0010,0x0000);
    // Wait 30ms      
    delay(3000);
    // set R07h at 0033h (GON=1,DTE=1,D[1:0]=11)
    write_reg(0x0007,0x0033);
    // Entry mode setting (R11h)
    // R11H Entry mode
    // vsmode DFM1 DFM0 TRANS OEDef WMode DMode1 DMode0 TY1 TY0 ID1 ID0 AM LG2 LG2 LG0
    //   0     1    1     0     0     0     0      0     0   1   1   1  *   0   0   0
    write_reg(0x0011,0x6070);
    // LCD driver AC setting (R02h)
    write_reg(0x0002,0x0600);
    // power control 1
    // DCT3 DCT2 DCT1 DCT0 BT2 BT1 BT0 0 DC3 DC2 DC1 DC0 AP2 AP1 AP0 0
    // 1     0    1    0    1   0   0  0  1   0   1   0   0   1   0  0
    // DCT[3:0] fosc/4 BT[2:0]  DC{3:0] fosc/4
    write_reg(0x0003,0x0804);//0xA8A4
    write_reg(0x000C,0x0000);//
    write_reg(0x000D,0x0808);// 0x080C --> 0x0808
    // power control 4
    // 0 0 VCOMG VDV4 VDV3 VDV2 VDV1 VDV0 0 0 0 0 0 0 0 0
    // 0 0   1    0    1    0    1    1   0 0 0 0 0 0 0 0
    write_reg(0x000E,0x2900);
    write_reg(0x001E,0x00B8);
    write_reg(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
    write_reg(0x0010,0x0000);
    write_reg(0x0005,0x0000);
    write_reg(0x0006,0x0000);
    write_reg(0x0016,0xEF1C);
    write_reg(0x0017,0x0003);
    write_reg(0x0007,0x0233);//0x0233
    write_reg(0x000B,0x0000|(3<<6));
    write_reg(0x000F,0x0000);//扫描开始地址
    write_reg(0x0041,0x0000);
    write_reg(0x0042,0x0000);
    write_reg(0x0048,0x0000);
    write_reg(0x0049,0x013F);
    write_reg(0x004A,0x0000);
    write_reg(0x004B,0x0000);
    write_reg(0x0044,0xEF00);
    write_reg(0x0045,0x0000);
    write_reg(0x0046,0x013F);
    write_reg(0x0030,0x0707);
    write_reg(0x0031,0x0204);
    write_reg(0x0032,0x0204);
    write_reg(0x0033,0x0502);
    write_reg(0x0034,0x0507);
    write_reg(0x0035,0x0204);
    write_reg(0x0036,0x0204);
    write_reg(0x0037,0x0502);
    write_reg(0x003A,0x0302);
    write_reg(0x003B,0x0302);
    write_reg(0x0023,0x0000);  
    write_reg(0x0024,0x0000);
    write_reg(0x0025,0x8000);   // 65hz
    write_reg(0x004f,0);        // 行首址0
    write_reg(0x004e,0);        // 列首址0
#else
	write_reg(0x0000,0x0001);		//打开晶振
	write_reg(0x0003,0xA8A4);		//0xA8A4
	write_reg(0x000C,0x0000);		
	write_reg(0x000D,0x080C);		
	write_reg(0x000E,0x2B00);		
	write_reg(0x001E,0x00B0);		
	write_reg(0x0001,0x2B3F);		//驱动输出控制320*240  0x693F
	write_reg(0x0002,0x0600);		//LCD Driving Waveform control
	write_reg(0x0010,0x0000);	 
	write_reg(0x0011,0x6070);		//0x4030	//定义数据格式	16位色	横屏 0x6058
	write_reg(0x0005,0x0000);	 
	write_reg(0x0006,0x0000);	 
	write_reg(0x0016,0xEF1C);	 
	write_reg(0x0017,0x0003);	 
	write_reg(0x0007,0x0233);		//0x0233	   
	write_reg(0x000B,0x0000);	 
	write_reg(0x000F,0x0000);		//扫描开始地址
	write_reg(0x0041,0x0000);	 
	write_reg(0x0042,0x0000);	 
	write_reg(0x0048,0x0000);	 
	write_reg(0x0049,0x013F);	 
	write_reg(0x004A,0x0000);	 
	write_reg(0x004B,0x0000);	 
	write_reg(0x0044,0xEF00);	 
	write_reg(0x0045,0x0000);	 
	write_reg(0x0046,0x013F);	 
	write_reg(0x0030,0x0707);	 
	write_reg(0x0031,0x0204);	 
	write_reg(0x0032,0x0204);	 
	write_reg(0x0033,0x0502);	 
	write_reg(0x0034,0x0507);	 
	write_reg(0x0035,0x0204);	 
	write_reg(0x0036,0x0204);	 
	write_reg(0x0037,0x0502);	 
	write_reg(0x003A,0x0302);	 
	write_reg(0x003B,0x0302);	 
	write_reg(0x0023,0x0000);	 
	write_reg(0x0024,0x0000);	 
	write_reg(0x0025,0x8000);	 
	write_reg(0x004e,0); 	   //列(X)首址0
	write_reg(0x004f,0); 	   //行(Y)首址0

#endif

    //数据总线测试,用于测试硬件连接是否正常.
    lcd_data_bus_test();  
    //GRAM测试,此测试可以测试LCD控制器内部GRAM.测试通过保证硬件正常
//    lcd_gram_test();
   
    //清屏  
    lcd_clear( Yellow );
}

/*  设置像素点 颜色,X,Y */
void ssd1289_lcd_set_pixel(const char* pixel, int x, int y)
{
    lcd_SetCursor(x,y);

    rw_data_prepare();
    write_data(*(rt_uint16_t*)pixel);
}  

  
/*  设置像素点(粗细) 颜色,X,Y,点的粗细z(点的大小) */
void ssd1289_lcd_set_pixel_size(const char* pixel, int x, int y, int cnt)
{ 
	u8 temp, temp1;
	
	for(temp = 0; temp < cnt; temp++){
		for(temp1=0; temp1 < cnt; temp1++){		
		 	ssd1289_lcd_set_pixel(&pixel, (x+temp), (y+temp1));
		} 	
	}
}


/* 获取像素点颜色 */
void ssd1289_lcd_get_pixel(char* pixel, int x, int y)
{
	*(rt_uint16_t*)pixel = lcd_read_gram(x, y);
}

/* 画水平线 */
void ssd1289_lcd_draw_hline(const char* pixel, int x1, int x2, int y)
{
    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0011,0x6030 | (0<<3)); // AM=0 hline

    lcd_SetCursor(x1, y);
    rw_data_prepare(); /* Prepare to write GRAM */
    while (x1 < x2)
    {
        write_data(*(rt_uint16_t*)pixel);
        x1++;
    }
}

/* 垂直线 */
void ssd1289_lcd_draw_vline(const char* pixel, int x, int y1, int y2)
{
    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0011,0x6070 | (1<<3)); // AM=0 vline

    lcd_SetCursor(x, y1);
    rw_data_prepare(); /* Prepare to write GRAM */
    while (y1 < y2)
    {
        write_data(*(rt_uint16_t*)pixel);
        y1++;
    }
}

/* blit a line */
void ssd1289_lcd_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
	rt_uint16_t *ptr;

	ptr = (rt_uint16_t*)pixels;

    /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
    write_reg(0x0011,0x6070 | (0<<3)); // AM=0 hline

    lcd_SetCursor(x, y);
    rw_data_prepare(); /* Prepare to write GRAM */
    while (size)
    {
        write_data(*ptr ++);
		size --;
    }
}

struct rt_device_graphic_ops ssd1289_ops =
{
	ssd1289_lcd_set_pixel,
	ssd1289_lcd_get_pixel,
	ssd1289_lcd_draw_hline,
	ssd1289_lcd_draw_vline,
	ssd1289_lcd_blit_line
};

struct rt_device _lcd_device;
static rt_err_t lcd_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t lcd_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	switch (cmd)
	{
	case RTGRAPHIC_CTRL_GET_INFO:
		{
			struct rt_device_graphic_info *info;

			info = (struct rt_device_graphic_info*) args;
			RT_ASSERT(info != RT_NULL);

			info->bits_per_pixel = 16;
			info->pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565P;
			info->framebuffer = RT_NULL;
			info->width = 240;
			info->height = 320;
		}
		break;

	case RTGRAPHIC_CTRL_RECT_UPDATE:
		/* nothong to be done */
		break;

	default:
		break;
	}

	return RT_EOK;
}

void rt_hw_lcd_init(void)
{
    /* LCD RESET */
    /* PF10 : LCD RESET */
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOF,&GPIO_InitStructure);

        GPIO_ResetBits(GPIOF,GPIO_Pin_10);
        GPIO_SetBits(GPIOF,GPIO_Pin_10);
        /* wait for lcd reset */
        rt_thread_delay(1);
    }

	/* register lcd device */
	_lcd_device.type  = RT_Device_Class_Graphic;
	_lcd_device.init  = lcd_init;
	_lcd_device.open  = lcd_open;
	_lcd_device.close = lcd_close;
	_lcd_device.control = lcd_control;
	_lcd_device.read  = RT_NULL;
	_lcd_device.write = RT_NULL;

	_lcd_device.user_data = &ssd1289_ops;
    ssd1289_init();

    /* register graphic device driver */
	rt_device_register(&_lcd_device, "lcd",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}




#if SSD1289_API_DEBUG    
void cmd_ssd1289_api(int cmd, int data, int data1, int data2, int data3)
{    
	u32 temp, temp1;
	switch (cmd) {
	case 0:
 		temp = read_reg(0x00);
		printf("\r\nLCD Device ID : %04X \n", temp);
		    
		break;
  
	case 1:
		temp = read_reg(0x0018);  
		printf("\r\nread  1st : %04X \n", temp);
      
		write_reg(0x0018,0x0039);
		printf("\r\nwrite 1st : %04X \n", temp);

		temp = read_reg(0x0018);
		printf("\r\nread 2st : %04X \n", temp);

		break;
  
	case 2:  
		temp1 = read_reg(0x0016);  
		write_reg(0x0016,0x0021);
		temp = read_reg(0x0016);
  
		printf("\r\nread 1st : %04X \n", temp1);
		printf("\r\nread 2st : %04X \n", temp);

		break;
  
	case 3:    
	    lcd_clear( White );  
		break;  

	case 4:        
	    lcd_clear( Green );
		break;

	case 5:       
		lcd_clear( Blue );
		break;
        
	case 6:    
		
		for(temp=0; temp<0xfff0; temp++){
			if((temp%(0xff)) == 0)
				lcd_clear(temp);
		}

		printf("opearation down...\n");  

		break;
  
	case 7:       
		while(1){
  
			temp += 50;
			lcd_clear(temp);
			delay(5000);  
		} 

		break;
  
	case 8:       
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		break;

	case 9:       
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		break;
    
	case 10:       
		ssd1289_lcd_set_pixel(data, data1, data2);
		break;

	case 11:	   
		ssd1289_lcd_draw_hline(data, data1, data2, data3);	
		break;
  
	case 12:	   
		ssd1289_lcd_draw_vline(data, data1, data2, data3);
		break;

	case 13:    
		ssd1289_lcd_set_pixel_size(data, data1, data2, data3);
		break;


	default:
		break;
	}
	
	return;
}
FINSH_FUNCTION_EXPORT(cmd_ssd1289_api, cmd_debug);
#endif 




