/********************************************************************************
********************************************************************************
* 权限:	杭州南江机器人股份有限公司
* 文件名:	  Timer.h 		
* 功能描述:	
*		  定时器头文件
* 版本 	 作者		时间		  状态
* V1.0 	 chenshouxian	2015.08.08	创建文件
*****************************************************************************
*****************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"

typedef u64 timer_t;

extern vu64 sys_tick;

/* export function */
void init_wait_timer(void);
void update_sys_timer(void);
void wait_ms(u16 time);
void wait_100us(u8 time);
void wait_us(u8 time);
u64 get_sys_tick(void);
void wait_ms_with_dis_int(u8 time);

timer_t get_one_timer(u32 time);
u8 check_timer(timer_t timer);


#endif

