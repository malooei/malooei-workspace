/*
 * Description:
 * am2301 is a temperature and humidity sensor
 * am2301 api.h
 *
 */
#define AM2301_API_H_

#include "stm32f10x.h"

#define AM2301_API_DEBUG 	1 
#define TIME_DELAY_30US		160
#define TIME_DELAY_600US	20 * TIME_DELAY_30US
#define TIME_DELAY_120US	4 * TIME_DELAY_30US
#define TIME_DELAY_50US		267
#define TIME_DELAY_10MS		320 * TIME_DELAY_30US

extern void read_ext_am2301(void);


