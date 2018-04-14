 /********************************************************************************
 ********************************************************************************
 * 权限: 杭州南江机器人股份有限公司
 * 文件名:	   action.h	 
 * 功能描述: 
 *		   头文件
 * 版本   作者		     时间		   状态
 * V1.0   chenshouxian	 2015.09.10  创建文件
 * V1.1   sunlifeng	     2016.04.8   修改文件
 *****************************************************************************
 *****************************************************************************/
#ifndef __ACTION_H__
#define __ACTION_H__

#include "typedef.h"
#include "packet.h"

#define SHOOT 0
#define CHIP 1

typedef struct _shooter_
{
	int strength;
	int count_down;
} shooter_t;

int on_robot_command( packet_robot_t *packet );
int do_robot_command( packet_robot_t *packet );
void EXTI0_IRQHandler(void);
int init_shooter(void);
int do_dribbler( int dribbler );
int do_move( int speed_x, int speed_y, int speed_rot );
int do_shoot( int shoot, int chip );
int do_chip(int shoot, int chip );
int update_shooter(void);
int set_test_shooter(void);

#endif
