/******************************************************************************
 * packet.h - 
 * 
 * Copyright 2008-2015 Shouxian Chen.
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0   2015/09/11, Shouxian Chen create this file
 * 
 ******************************************************************************/
#ifndef __PACKET_H__
#define __PACKET_H__

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "cfg.h"

#define DEBUG_FUNCTION_Normal 	            0
#define DEBUG_FUNCTION_Set_9557_error       7
#define MAX_IDENTIFY_LEN                    256
#define IDENTIFY_START_ADDR                 21   //packet中第22个字节表示认证识别地址开始
typedef enum _packet_type_
{
	PACKET_NONE = -1,
	PACKET_Normal = 0,
	PACKET_DEBUG_Check_Frq = 2,
	PACKET_DEBUG_Set_9557_error = 7
} packet_type_t;

typedef struct _packet_robot_
{
	int robot_num; //robot id
	
	int dribbler;  //控球档位
	int shoot;     //平射
	int chip;      //挑射
	int speed_x;   //小车整体x轴方向速度 单位[cm/s] X轴为吸球嘴巴方向
	int speed_y;   //y轴方向速度 单位[cm/s]
	int speed_rot; //旋转方向速度  单位[1表示0.25rad/s]
	
} packet_robot_t;

/**cpuid接收缓存一个加密的cpuid为8Byte 最大支持32个robot 同时进行cpuid认证**/
typedef struct
{
    unsigned char   recv_cpuid[MAX_IDENTIFY_LEN];
	unsigned short  recv_packet_cnt;              //几个robot的认证cpuid 所需要传输的包个数
	unsigned char   recv_cpuid_start_flag;        //开始接收cpuid数据标志
	unsigned char   recv_cpuid_ok;               //cpuid认证码全部接收完毕标志
}idenfity_cpuid_struct;

packet_type_t get_packet_type( unsigned char* data, int len );
int stop_mode_packet(char *q);
int packet(char *q);
int decode_packet_robot( packet_robot_t *packet, unsigned char *data, int len );
int decode_identify_packet( idenfity_cpuid_struct *id_code, unsigned char *data );


#endif
