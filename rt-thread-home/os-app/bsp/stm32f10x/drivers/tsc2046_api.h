/*
 * Description:
 * tsc2046 api.h
 *
 */
#define TSC2046_API_H_

#include "stm32f10x.h"

#define TSC2046_API_DEBUG 1  

#define BIT_R_SHIFT(data, n)  data>>(n)
#define BIT_SET		1
#define BIT_CLR		0
#define POINT_NUM 	4

/*  
 *	校准触摸屏参数
 */
enum touch_screen_point_e {
	X_SCREEN = 0,
	Y_SCREEN,
	X_TOUCH,
	Y_TOUCH,
};

/* 屏幕对应的点坐标 */
struct orig_screen_xy_st {
	u32 x_position_orig;
	u32 y_position_orig;
};

/* 触摸屏得到的坐标 */
struct stouch_screen_xy_st {
	u32 x_position;
	u32 y_position;
};


/* 坐标映射 */
struct remap_point_st {
	struct orig_screen_xy_st;
	struct stouch_screen_xy_st;
};

struct calc_touch_screen_st {
	struct remap_point_st remap_st[POINT_NUM];
};


extern struct calc_touch_screen_st calctouch;



