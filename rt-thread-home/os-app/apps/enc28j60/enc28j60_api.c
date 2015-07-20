#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <enc28j60_hw.h>
#include <enc28j60_api.h>

/* 移植了enc28j60_io.c文件form rt-thread references工程 */


/*
 * 粗略延时
 * delay 30us -> ncount -> 160
 * delay 50us -> ncount -> 267
 */
#define ENC28J60_DELAY_50US 267
#define ENC28J60_DELAY_1MS  (20*ENC28J60_DELAY_50US)
#define ENC28J60_DELAY_MS_CNT 10301
#define ENC28J60_DELAY_US_CNT 11
#define DUMMY_DATA4ENC28J60 (0xc0)
  

/*
 * 参见enc28j60数据手册:表4-1 enc28j60 spi指令集
 * 读控制寄存器(RCR)       操作码 000(bit7~bit5)  参数 aaaaa(bit4~bit0)
 * 读缓冲器(RBM)           操作码 001(bit7~bit5)  参数 11010(bit4~bit0)
 * 写控制寄存器(WCR)       操作码 010(bit7~bit5)  参数 aaaaa(bit4~bit0)
 * 写缓冲器(WBM)           操作码 011(bit7~bit5)  参数 11010(bit4~bit0)
 * 位域置1(BFS)            操作码 100(bit7~bit5)  参数 aaaaa(bit4~bit0)
 * 位域清0(BFC)            操作码 101(bit7~bit5)  参数 aaaaa(bit4~bit0)
 * 系统命令(软件复位)(SC)  操作码 111(bit7~bit5)  参数 11111(bit4~bit0)
 * \author malooei
 */

/* BFS -> bit filed set */
#define enc28j60_bfs(address)		enc28j60_readwrite_byte(0x80 | (address))
/* BFC -> bit filed clr */
#define enc28j60_bfc(address)		enc28j60_readwrite_byte(0xa0 | (address))
/* RCR -> read control register */
#define enc28j60_rcr(address)		enc28j60_readwrite_byte(0x00 | (address))
/* RBM -> read buffer memory */
#define enc28j60_rbm() 			enc28j60_readwrite_byte(0x3a)
/* WCR -> write control register */
#define enc28j60_wcr(address)		enc28j60_readwrite_byte(0x40 | (address))
/* WBM -> write buffer memory */  
#define enc28j60_wbm(address)		enc28j60_readwrite_byte(0x7a)
/* soft reset */
#define enc28j60_sc() 			enc28j60_readwrite_byte(0xff)


#define spi_send_byte(data)		enc28j60_readwrite_byte(data)
//#define spi_rec_byte()		spix_rec_byte(ENC28J60_SPIX, DUMMY_DATA4ENC28J60)

#define spi_send_data(data, len)	spix_send_data(ENC28J60_SPIX, data, len)
#define spi_rec_data(buf, len)		spix_rec_data(ENC28J60_SPIX, buf, len, DUMMY_DATA4ENC28J60)


  
static void enc28j60_delay(vu32 ncount)    //延时函数
{
  for(; ncount > 0; ncount--);
}


/* ms级延时函数没有实际测试 */
static void enc28j60_delay_delay_ms(u32 nMs)
{
	u32 i;

	for(; nMs !=0; nMs--) {
		//i = 10301;
		i = ENC28J60_DELAY_MS_CNT;
		while(i--);
	}
}


/* ms级延时函数没有实际测试 */
static void enc28j60_delay_delay_us(u32 nMs)
{
	u32 i;

	for(; nMs !=0; nMs--) {
		//i = 11;
		i = ENC28J60_DELAY_US_CNT;
		while(i--);
	}
}


/*
 * hardware to reset the ENC28J60
 */
void enc28j60_reset_hardware() 
{  
	ENC28J60_CS_LOW;
	enc28j60_delay(ENC28J60_DELAY_1MS);
	ENC28J60_CS_HIGH;
	enc28j60_delay(ENC28J60_DELAY_1MS);
}
 

/*
 * software to reset the ENC28J60
 */
void enc28j60_soft_reset()
{
	/* write to address */
	ENC28J60_CS_LOW; 
	enc28j60_sc();
	enc28j60_readwrite_byte(0xff);
	ENC28J60_CS_HIGH;  
}


/**
 * Switches the hardware register bank.
 *
 * \param[in] num The index of the register bank to switch to.
 */
void enc28j60_bank(u8 num)
{
	static u8 bank = 0xff;

	if(num == bank)
		return;

	/* clear bank bits */
	ENC28J60_CS_LOW;
	enc28j60_bfc(ECON1);
	enc28j60_readwrite_byte(0x03);
	ENC28J60_CS_HIGH;

	/* set bank bits */
	ENC28J60_CS_LOW;
	enc28j60_bfs(ECON1);
	enc28j60_readwrite_byte(num);
	ENC28J60_CS_HIGH;

	bank = num;
}


/*
 * reads the value of a hardware register.
 *
 * return: register value.
 */
u8 enc28j60_read(u8 address)
{
	u8 value;

	/* switch to register bank */
	enc28j60_bank((address & MASK_BANK) >> SHIFT_BANK);

	/* read from address */
	ENC28J60_CS_LOW;
	enc28j60_rcr((address & MASK_ADDR) >> SHIFT_ADDR);
	if(address & MASK_DBRD)
		enc28j60_readbyte();
	value = enc28j60_readbyte();
	ENC28J60_CS_HIGH;

	return value;
} 


/**
 * \internal
 * Writes the value of a hardware register.
 *
 * \param[in] address The address of the register to write.
 * \param[in] value The value to write into the register.
 */
void enc28j60_write(u8 address, u8 value)
{
	/* switch to register bank */
	enc28j60_bank((address & MASK_BANK) >> SHIFT_BANK);

	/* write to address */
	ENC28J60_CS_LOW;
	enc28j60_wcr((address & MASK_ADDR) >> SHIFT_ADDR);
	enc28j60_readwrite_byte(value);
	ENC28J60_CS_HIGH;  
}




/**
 * \internal
 * Clears bits in a hardware register.
 *
 * Performs a NAND operation on the current register value
 * and the given bitmask.
 *
 * \param[in] address The address of the register to alter.
 * \param[in] bits A bitmask specifiying the bits to clear.
 * bit0~bit7
 */
void enc28j60_clear_bits(u8 address, u8 bits)
{
	/* switch to register bank */
	enc28j60_bank((address & MASK_BANK) >> SHIFT_BANK);

	/* write to address */
	ENC28J60_CS_LOW;
	enc28j60_bfc((address & MASK_ADDR) >> SHIFT_ADDR);
	spi_send_byte(bits);
	ENC28J60_CS_HIGH;
}


/**
 * \internal
 * Sets bits in a hardware register.
 *
 * Performs an OR operation on the current register value
 * and the given bitmask.
 *
 * \param[in] address The address of the register to alter.
 * \param[in] bits A bitmask specifiying the bits to set.
 * bit0~bit7
 */
void enc28j60_set_bits(u8 address, u8 bits)
{
	/* switch to register bank */
	enc28j60_bank((address & MASK_BANK) >> SHIFT_BANK);

	/* write to address */
	ENC28J60_CS_LOW;
	enc28j60_bfs((address & MASK_ADDR) >> SHIFT_ADDR);
	spi_send_byte(bits);
	ENC28J60_CS_HIGH;
}


#if 0
/* 待测试 mark by malooei */
/**
 * \internal
 * Reads multiple bytes from the RAM buffer.
 *
 * \param[out] buffer A pointer to the buffer which receives the data.
 * \param[in] buffer_len The buffer length and number of bytes to read.
 */
void enc28j60_read_buffer(u8* buffer, u16 buffer_len)
{
	ENC28J60_CS_LOW;

	enc28j60_rbm();
	spi_rec_data(buffer, buffer_len);

	ENC28J60_CS_HIGH;
}
  

/**
 * \internal
 * Writes multiple bytes to the RAM buffer.
 *
 * \param[in] buffer A pointer to the buffer containing the data to write.
 * \param[in] buffer_len The number of bytes to write.
 */
void enc28j60_write_buffer(u8* buffer, u16 buffer_len)
{
	ENC28J60_CS_LOW;

	enc28j60_wbm();
	spi_send_data(buffer, buffer_len);

	ENC28J60_CS_HIGH;
}

#else
    
/**
 * \internal
 * Reads the value of a hardware PHY register.
 *
 * \param[in] address The address of the PHY register to read.
 * \returns The register value.
 */
#define DEBUG_PHY_READ 1
u16 enc28j60_read_phy(u8 address)
{
#if 1

#if DEBUG_PHY_READ
	s8 temp;  
#endif 

	
	u8 val_h, val_l;

	enc28j60_write(MIREGADR, address);

#if DEBUG_PHY_READ
	temp = enc28j60_read(MICMD); 
	rt_kprintf("befroe set bit: %d \n", temp);
#endif
 
	enc28j60_write(MICMD, 1);

	//_delay_us(10);
	//enc28j60_delay_delay_us(20);
	enc28j60_delay(100);   

#if DEBUG_PHY_READ
	temp = enc28j60_read(MICMD); 
	rt_kprintf("after set bit: %d \n", temp);
#endif
 
	while(enc28j60_read(MISTAT) & (1 << MISTAT_BUSY));

#if DEBUG_PHY_READ
	temp = enc28j60_read(MICMD); 
	rt_kprintf("before clr bit: %d \n", temp);
#endif  
  
	enc28j60_write(MICMD, 0);
  
#if DEBUG_PHY_READ
	temp = enc28j60_read(MICMD); 
	rt_kprintf("after clr bit: %d \n", temp);
#endif


	val_h = enc28j60_read(MIRDH);
	val_l = enc28j60_read(MIRDL);

	rt_kprintf("val_h: %d  val_l: %d\n", val_h, val_l);

	    
	return ((u16) enc28j60_read(MIRDH)) << 8 |
		   ((u16) enc28j60_read(MIRDL));
#else
	
		
		u8 val_h, val_l;
	
		enc28j60_write(MIREGADR, address);
	
	
		enc28j60_set_bits(MICMD, (1 << MICMD_MIIRD));
	
		//_delay_us(10);
		//enc28j60_delay_delay_us(20);
		enc28j60_delay(100);   
	
	 
		while(enc28j60_read(MISTAT) & (1 << MISTAT_BUSY));
	
	
		enc28j60_clear_bits(MICMD, (1 << MICMD_MIIRD));
	  
	
	
		val_h = enc28j60_read(MIRDH);
		val_l = enc28j60_read(MIRDL);
	
		rt_kprintf("val_h: %d  val_l: %d\n", val_h, val_l);
	
		    
		return ((u16) enc28j60_read(MIRDH)) << 8 |
			   ((u16) enc28j60_read(MIRDL));

#endif
}

/**
 * \internal
 * Writes the value to a hardware PHY register.
 *
 * \param[in] address The address of the PHY register to write.
 * \param[in] value The value to write into the register.
 */
void enc28j60_write_phy(u8 address, u16 value)
{
#if 1
	enc28j60_write(MIREGADR, address);
	enc28j60_write(MIWRL, value & 0xff);
	enc28j60_write(MIWRH, value >> 8);

	//_delay_us(10);
	//enc28j60_delay_delay_us(20);
	enc28j60_delay(100);  


	while(enc28j60_read(MISTAT) & (1 << MISTAT_BUSY));
#else
	enc28j60_write(MIREGADR, address);
	enc28j60_write(MIWRL, value & 0xff);
	enc28j60_write(MIWRH, value >> 8);

	//_delay_us(10);
	enc28j60_delay_delay_us(20);

	while(enc28j60_read(MISTAT) & (1 << MISTAT_BUSY));
#endif
}

#endif



#if 0



/**
 * \internal
 * Reads a byte from the RAM buffer at the current position.
 *
 * \returns The byte read from the current RAM position.
 */
uint8_t enc28j60_read_buffer_byte()
{
	uint8_t b;

	enc28j60_select();
	enc28j60_rbm();

	b = spi_rec_byte();

	enc28j60_deselect();

	return b;
}

/**
 * \internal
 * Writes a byte to the RAM buffer at the current position.
 *
 * \param[in] b The data byte to write.
 */
void enc28j60_write_buffer_byte(uint8_t b)
{
	enc28j60_select();
	enc28j60_wbm();

	spi_send_byte(b);

	enc28j60_deselect();
}



#endif
  
#if ENC28J60_API_DEBUG    
void cmd_enc28j60_api(int cmd, int data, int data1)
{    
	u32 temp;
	switch (cmd) {
	case 0:
		
		break;

	case 1:
		break;

	case 2:
		break;

	case 3:  
		break;

	case 4:       
		break;

	default:
		break;
	}
	
	return;
}
FINSH_FUNCTION_EXPORT(cmd_enc28j60_api, cmd_debug);
#endif 

  
