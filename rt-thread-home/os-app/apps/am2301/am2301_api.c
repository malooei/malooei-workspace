#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <am2301_hw.h>
#include <am2301_api.h>

struct am2301_sample_st am2301_member;


void am2301_delay(vu32 ncount)    //��ʱ����
{
  
  for(; ncount > 0; ncount--);

}

  
static void ready_to_rx_data(void)
{
	u32 vlaue;

	/* �ȴ��͵�ƽͨ�� */
	waitlow_data_passby();

	/* �ȴ��ߵ�ƽͨ�� */
	waithigh_data_passby();

	/* �ȴ��͵�ƽͨ�� */
	waitlow_data_passby();	
}

/*
 *
 * ����am2301����
 * ����am2301_member��
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

			/* �ȴ��ߵ�ƽͨ�� */   
			waithigh_data_passby();

			/* �ȴ��͵�ƽͨ�� */
			waitlow_data_passby();	
			    
		}else{
			if(cnt<16){
				am2301_member.humidity = am2301_member.humidity << 1;

  			}else if((cnt >= 16)&(cnt < 32)){
				am2301_member.temp = am2301_member.temp << 1;

			}else if(cnt >= 32){
				am2301_member.check = am2301_member.check << 1;

			}    

			/* �ȴ��͵�ƽͨ�� */
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
  //turnlow_am2301_data(); /* �ֲ����ⲿ�ֵ�ʱ���г��룬�ᵽʱ�ӻ����ݷ��ص�ƽ���� */

  ready_to_rx_data();	  

  start_to_rx_data();

  check_the_rx_data();
}

  
/*
 *  ע��1:������DHT21��ȡ����ʪ����������ǰһ�εĲ���ֵ
 *      2:ע:�������ڼ�����õ���1.7���ӣ�����2�룩
 *      3:�����β���֮��û����ʱ�����ɴӻ��ڵڶ���ͨ�Ź�����û�з�Ӧ������ϵͳ������ѭ������Ϊ������û�мӳ�ʱ�ж�
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

 

  
