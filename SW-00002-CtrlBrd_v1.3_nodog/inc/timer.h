/********************************************************************************
********************************************************************************
* Ȩ��:	�����Ͻ������˹ɷ����޹�˾
* �ļ���:	  Timer.h 		
* ��������:	
*		  ��ʱ��ͷ�ļ�
* �汾 	 ����		ʱ��		  ״̬
* V1.0 	 chenshouxian	2015.08.08	�����ļ�
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

