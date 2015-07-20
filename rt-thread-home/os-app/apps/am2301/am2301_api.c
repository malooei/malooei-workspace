#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <am2301_hw.h>
#include <am2301_api.h>

struct am2301_sample_st am2301_member;


void am2301_delay(vu32 ncount)    //延时函数
{
  
  for(; ncount > 0; ncount--);

}

  
static void ready_to_rx_data(void)
{
	u32 vlaue;

	/* 等待低电平通过 */
	waitlow_data_passby();

	/* 等待高电平通过 */
	waithigh_data_passby();

	/* 等待低电平通过 */
	waitlow_data_passby();	
}

/*
 *
 * 接收am2301数据
 * 更新am2301_member中
 *
 */
static void start_to_rx_data(void)
{
	u8 cnt;
	u32 flag;


	am2301_member.humidity = 0;
	am2301_member.temp = 0;
	am2301_member.check = 0;

	for(cnt = 0; cnt < 40; cnt++){
		  
		am2301_delay(TIME_DELAY_50US);

		flag = GPIO_ReadInputDataBit(AM2301_DATA_PORT, AM2301_DATA_PIN);

		if(1 == flag){
			if(cnt<16){
				am2301_member.humidity = am2301_member.humidity << 1;
				am2301_member.humidity = am2301_member.humidity | 0x1;
  			}else if((cnt >= 16)&(cnt < 32)){
				am2301_member.temp = am2301_member.temp << 1;
				am2301_member.temp = am2301_member.temp | 0x1;

			}else if(cnt >= 32){
				am2301_member.check = am2301_member.check << 1;
				am2301_member.check = am2301_member.check | 0x1;

			}

			/* 等待高电平通过 */   
			waithigh_data_passby();

			/* 等待低电平通过 */
			waitlow_data_passby();	
			    
		}else{
			if(cnt<16){
				am2301_member.humidity = am2301_member.humidity << 1;

  			}else if((cnt >= 16)&(cnt < 32)){
				am2301_member.temp = am2301_member.temp << 1;

			}else if(cnt >= 32){
				am2301_member.check = am2301_member.check << 1;

			}    

			/* 等待低电平通过 */
			waitlow_data_passby();
		}  

	}

}

  
static void check_the_rx_data(void)
{
	u32 value;

	value = am2301_member.humidity/256 + am2301_member.humidity%256 + am2301_member.temp/256 + am2301_member.temp%256;
	if((value%256) == am2301_member.check){
		rt_kprintf("read am2301 succ...\n");		
	}else{
		rt_kprintf("read am2301 fail...\n");		
	}   
}
  
static void read_ext_am2301_once(void)
{
  turnlow_am2301_data();
  am2301_delay(TIME_DELAY_600US);
  turnhigh_am2301_data();
  am2301_delay(TIME_DELAY_30US); 
  //turnlow_am2301_data(); /* 手册上这部分的时序有出入，会到时从机数据返回电平过低 */

  ready_to_rx_data();	  

  start_to_rx_data();

  check_the_rx_data();
}

  
/*
 *  注：1:主机从DHT21读取的温湿度数据总是前一次的测量值
 *      2:注:采样周期间隔不得低于1.7秒钟（建议2秒）
 *      3:若两次采样之间没有延时则会造成从机在第二次通信过程中没有反应，导致系统跳入死循环，因为程序中没有加超时判断
 */   
void read_ext_am2301(void)
{
#if AM2301_API_DEBUG 
	rt_kprintf("test read am2301...\n");
#endif    

#if 1  
	read_ext_am2301_once();
#else
	read_ext_am2301_once();
	am2301_delay(200*TIME_DELAY_10MS); 
 	read_ext_am2301_once();
#endif
   
#if AM2301_API_DEBUG
	rt_kprintf("am2301_sample_st:\n temp:%d humidity:%d check:%d\n\n\n", am2301_member.temp, am2301_member.humidity, am2301_member.check);
#endif    
	  
}

 

  
