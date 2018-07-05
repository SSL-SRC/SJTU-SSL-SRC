/********************************************************************************
********************************************************************************
* 权限:	杭州南江机器人股份有限公司
* 文件名:	  Timer.c 		
* 功能描述:	
*		  定时器相关函数
* 版本 	 作者		时间		  状态
* V1.0 	 chenshouxian	2015.08.08	创建文件
*****************************************************************************
*****************************************************************************/

#include "timer.h"
#include "typedef.h"

/* global variable define */

/* system tick counter, all the time relationship operation is based on this */
vu64 sys_tick = 0;



/*******************************************************************************
* Function Name   : init_wait_timer
* Description	  : 初始化time6
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void init_wait_timer(void)
{
	/* TIM6_ARR is not buffered, one pulse mode */
	TIM6->CR1 = 0x0008;
	TIM6->CR2 = 0x0000;

	/* disable INT */
	TIM6->DIER = 0x0;

	/* prescaler X1 */
	TIM6->PSC = 0x0;

	TIM6->ARR = 0xffff;

	/* reload ARR and PSC */
	TIM6->EGR = 0x1;

	TIM6->CNT = 0x0;

	/* clear UIF bit */
	TIM6->SR = 0x0;
}


/*******************************************************************************
* Function Name   : update_sys_timer
* Description	  : 更新计数器变量
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void update_sys_timer(void)
{
	/* not need disable int, because the same int of ARM cortex-M3 cannot
	   be nested */
	sys_tick = sys_tick + 1;
}


/*******************************************************************************
* Function Name   : wait_ms
* Description	  : ms延迟函数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void wait_ms(u16 time)
{
	u64 time_out_tick;
	u64 tmp;

	DIS_INT();
	time_out_tick = sys_tick + time;
	EN_INT();

	while(1)
	{
		DIS_INT();
		tmp = sys_tick;
		EN_INT();

		if(tmp >= time_out_tick) break;
	}
}


/*******************************************************************************
* Function Name   : wait_100us
* Description	  : 延迟函数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void wait_100us(u8 time)
{
	while(time > 0)
	{
		wait_us(100);
		time--;
	}
}

/*******************************************************************************
* Function Name   : wait_us
* Description	  : us延迟函数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void wait_us(u8 time)
{
	TIM6->ARR = 42 * time - 20;
	TIM6->CNT = 0x0;
	TIM6->SR = 0x0;
	TIM6->CR1 |= 0x1;

	while((TIM6->CR1 & 0x1) == 1) ;
}



/*******************************************************************************
* Function Name   : wait_ms_with_dis_int
* Description	  : 延迟函数
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void wait_ms_with_dis_int(u8 time)
{
	u16 cnt;

	cnt = time * 4;
	while(cnt--)
	{
		wait_us(250);
	}
}


/*******************************************************************************
* Function Name   : get_sys_tick
* Description	  : Get the current system tick number
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
u64 get_sys_tick(void)
{
	u64 tmp;

	DIS_INT();
	tmp = sys_tick;
	EN_INT();

	return tmp;
}

/*******************************************************************************
* Function Name   : get_one_timer
* Description	  : get one timer from system
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
timer_t get_one_timer(u32 time)
{ 
	return get_sys_tick() + time;
}

/*******************************************************************************
* Function Name   : check_timer
* Description	  : check if the timer is expired
*					
* Input		      : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
u8 check_timer(timer_t timer)
{
	u64 cur_time;

	cur_time = get_sys_tick();
	
	if(cur_time >= timer) return 1;
	else return 0;
}


