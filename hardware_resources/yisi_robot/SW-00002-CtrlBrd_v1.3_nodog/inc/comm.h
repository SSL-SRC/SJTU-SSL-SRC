/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:	 comm.h 	   
* 功能描述:    
		 
* 版本		作者	   时间 		 状态
* V1.0		shouxian   2015.9.11   创建文件
* V1.1		sunlifeng  2016.2.28   修改文件
*****************************************************************************
*****************************************************************************/

#ifndef __COMMM_H__
#define __COMMM_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"
#include "nrf24l01.h"

#define MAX_BUFFER_LEN 25
#define nRF2401_BUFFER_LEN 256

#define JOYSTICK_POS_LIMIT 0xa0  //摇杆模拟量中间死区阈值  0x80为理论中间点
#define JOYSTICK_NEG_LIMIT 0x60
#define ROBOT_HIGH_SPEED_INCREMENT 0x80
#define JOYSTICK_MAX_SPEED 256  //摇杆摇到底的最大速度 x向y向速度单位[1cm/s] 旋转单位[0.025rad/s]

typedef struct _comm_
{
	unsigned char buffer[MAX_BUFFER_LEN];
	unsigned char buffer_pos;

	unsigned char status;

	int packet_error; //count number of bad packet received

}rf_comm_t;

int init_comm(void);
int do_comm(void);
int do_comm_up(void);
int do_packet_process( unsigned char *data, int len );
int do_joystick_packet( unsigned char *data);

extern rf_comm_t g_rf_comm;

#endif
